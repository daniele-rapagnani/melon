#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <cctype>
#include <cassert>
#include <vector>
#include <unordered_map>

#include "melon/libs/cxxopts/include/cxxopts.hpp"

extern "C" {
#include "melon/core/vm.h"
#include "melon/core/opcodes.h"
#include "melon/core/tstring.h"
#include "melon/core/serializer.h"
#include "melon/core/function.h"
}

namespace melon {

enum class MelTokenType
{
    NONE,
    INTEGER,
    NUMBER,
    STRING,
    SYMBOL,
    DOT,
    AT,
    COLON,
    PLUS
};

struct Token
{
    Token() = default;

    Token(MelTokenType type, uint32_t col, uint64_t line, const std::string& value)
        : type(type), col(col), line(line), value(value)
    { }

    MelTokenType type = MelTokenType::NONE;
    uint32_t col = 0;
    uint64_t line = 0;
    std::string value;
};

struct ConstantSymbol
{
    std::string name = "";
    Token value = {};
};

struct JmpEntry
{
    std::string label = "";
    TSize offset;
};

struct FunctionContext
{
    FunctionContext* getCurrentFunction()
    {
        if (this->functions.empty())
        {
            return nullptr;
        }

        return &this->functions.back();
    }

    FunctionContext& addNewFunction(VM* vm, const std::string& label)
    {
        this->functionLabels[label] = this->functions.size();
        this->functions.emplace_back();
        this->functions.back().func = melNewFunction(vm);

        return this->functions.back();
    }

    GCItem* func = nullptr;
    std::string funcLabel = {};
    std::unordered_map<std::string, TSize> labels = {};
    std::vector<JmpEntry> jmpTable = {};
    std::unordered_map<std::string, TConstantID> constantsMap = {};
    std::vector<ConstantSymbol> constants = {};
    std::unordered_map<std::string, TSize> functionLabels = {};
    std::vector<FunctionContext> functions = {};
    std::vector<UpvalueInfo> upvalues = {};
};

class Assembler 
{
public:
    Assembler(const std::string& fileName, const std::string& outFile)
    : fileName(fileName), outFile(outFile)
    { 
        this->outFileStr.open(this->outFile, std::ios_base::out | std::ios_base::binary);

        if (!this->outFileStr.is_open())
        {
            error("Error opening file for writing: " + this->outFile);
            return;
        }

        this->serializer.writer = [] (void* ctx, const void* data, TSize size) -> TRet
        {
            Assembler* as = reinterpret_cast<Assembler*>(ctx);
            as->outFileStr.write(reinterpret_cast<const char*>(data), size);

            return 0;
        };

        this->serializer.error = this->codeSerializer.error = [] (void* ctx, const char* error)
        {
            Assembler* as = reinterpret_cast<Assembler*>(ctx);
            as->error(error);
        };

        this->codeSerializer.writer = [] (void* ctx, const void* data, TSize size) -> TRet
        {
            Assembler* as = reinterpret_cast<Assembler*>(ctx);
            
            if (as->curBuffer + size >= as->buffer.size())
            {
                as->buffer.resize((as->buffer.size() + size) * 2);
            }

            memcpy(&as->buffer[as->curBuffer], data, size);
            as->curBuffer += size;

            return 0;
        };

        this->serializer.ctx = this;
        this->codeSerializer.ctx = this;

        VMConfig config = {};

        if (melCreateVM(&this->vm, &config) != 0)
        {
            error("Error initializing VM for assembling");
            return;
        }

        melSetPauseGC(&this->vm, &this->vm.gc, 1);
    }

    ~Assembler()
    {
        melSetPauseGC(&this->vm, &this->vm.gc, 0);
        melTriggerGC(&this->vm, &this->vm.gc);

        this->outFileStr.close();
    }

public:
    bool tokenize()
    {
        std::ifstream infile;
        infile.open(this->fileName, std::ifstream::in);

        if (!infile.is_open())
        {
            error("Can't open requested file: " + this->fileName);
            return false;
        }

        char c = '\0';
        std::string token = "";

        auto peek = [this, &infile] () -> char {
            char c = infile.peek();

            if (c == EOF)
            {
                return '\0';
            }

            return c;
        };
        
        auto get = [this, &infile] {
            char c = infile.get();

            if (c == EOF)
            {
                return '\0';
            }

            if (c == '\n')
            {
                this->col = 0;
                this->line++;
            }
            else
            {
                this->col++;
            }
            
            return c;
        };

        auto pushToken = [this, &token] (MelTokenType type) mutable {
            this->tokens.push_back(Token{
                type,
                this->col,
                this->line,
                token
            });

            token.assign("");
        };

        auto consumeWS = [&get, &infile, &peek] {
            while(peek() && std::iswspace(peek()))
            {
                get();
            }
        };

        while ((c = get()))
        {
            if (std::iswspace(c))
            {
                if (!token.empty())
                {
                    pushToken(MelTokenType::SYMBOL);
                }

                consumeWS();

                c = get();

                if (!c)
                {
                    break;
                }
            }

            if (c == '#')
            {
                while((c = peek()))
                {
                    if (peek() == '\n')
                    {
                        break;
                    }

                    get();
                }
            }
            else if (c == '"')
            {
                while ((c = peek()))
                {
                    if (c == '"')
                    {
                        get();
                        break;
                    }

                    token += get();
                }

                if (c != '\"')
                {
                    error("Unclosed string literal");
                    infile.close();
                    return false;
                }

                pushToken(MelTokenType::STRING);
            }
            else if (
                (((c == '.' || c == '-' || c == '+') && std::isdigit(peek()))
                || std::isdigit(c)) && token.empty()
            )
            {
                bool foundDot = false;
                uint16_t pos = 0;

                token += c;

                while((c = peek()))
                {
                    if (!std::isdigit(c) && (c != '.' || foundDot))
                    {
                        break;
                    }
                    
                    if (c == '.')
                    {
                        foundDot = true;
                    }

                    token += get();
                }

                pushToken(foundDot ? MelTokenType::NUMBER : MelTokenType::INTEGER);
            }
            else if (c == '@')
            {
                token = "@";
                pushToken(MelTokenType::AT);
            }
            else if (c == ':')
            {
                token = ":";
                pushToken(MelTokenType::COLON);
            }
            else if (c == '.')
            {
                token = ".";
                pushToken(MelTokenType::DOT);
            }
            else if (c == '+')
            {
                token = "+";
                pushToken(MelTokenType::PLUS);
            }
            else if (std::isalnum(c) || c == '_')
            {
                token += c;
            }
            else
            {
                std::stringstream ss;
                ss << "Invalid token: '" << c << "'";
                error(ss.str());
                infile.close();
                return false;
            }
        }

        if (!token.empty())
        {
            pushToken(MelTokenType::SYMBOL);
        }

        infile.close();

        return true;
    }

    Token* peekToken(TSize offset = 0)
    {
        if (curToken + offset >= this->tokens.size())
        {
            return nullptr;
        }
        
        return &this->tokens[curToken + offset];
    }

    Token* consumeToken(MelTokenType type) 
    {
        Token* token = peekToken();

        if (!token)
        {
            std::stringstream ss;
            ss << "Expected token " << static_cast<int>(type) << " but got nothing.";
            error(ss.str());
            
            return nullptr;
        }

        if (type != MelTokenType::NONE && token->type != type)
        {
            std::stringstream ss;
            ss << "Got unexpected token " << static_cast<int>(token->type) << " (" << token->value << "), expected: " << static_cast<int>(type);
            error(ss.str());

            return nullptr;
        }

        this->curToken++;

        return token;
    };

    bool consumeDataSection(FunctionContext& fc)
    {
        Token* tok = peekToken();
        Token* tokSym = peekToken(1);

        if (!tok || tok->type != MelTokenType::DOT || !tokSym || tokSym->type != MelTokenType::SYMBOL || tokSym->value != "data")
        {
            return true;
        }

        consumeToken(MelTokenType::DOT);
        consumeToken(MelTokenType::SYMBOL);

        while(peekToken())
        {
            if (peekToken()->type != MelTokenType::SYMBOL)
            {
                break;
            }

            Token* tok = consumeToken(MelTokenType::SYMBOL);
            Token* value = consumeToken(MelTokenType::NONE);

            if (
                value->type != MelTokenType::INTEGER
                &&  value->type != MelTokenType::NUMBER
                &&  value->type != MelTokenType::STRING
            )
            {
                error("Unexpected token, expected a constant value, got: " + value->value);
                return false;
            }

            ConstantSymbol cs;
            cs.name = tok->value;
            cs.value = *value;

            fc.constants.push_back(std::move(cs));
            fc.constantsMap[tok->value] = fc.constants.size() - 1;
        }

        return true;
    }

    bool compilePushK(FunctionContext& fc)
    {
        Token* tok = consumeToken(MelTokenType::SYMBOL);

        if (!tok)
        {
            error("Expected symbol token but got nothing");
            return false;
        }

        if (fc.constantsMap.find(tok->value) == fc.constantsMap.end())
        {
            error("Undefined constant: '" + tok->value + "'");
            return false;
        }

        TConstantID id = fc.constantsMap[tok->value];

        return melSerializerWriteOp1_25(&this->codeSerializer, MELON_OP_PUSHK, id) == 0;
    }

    bool compileSingleSignedArg(TVMOpcode op)
    {
        Token* tok = consumeToken(MelTokenType::INTEGER);

        if (!tok)
        {
            error("Expected argument for instruction");
            return false;
        }

        return melSerializerWriteOp1_s25(&this->codeSerializer, op, atoi(tok->value.c_str())) == 0;
    }

    bool compileSingleArg(TVMOpcode op)
    {
        Token* tok = consumeToken(MelTokenType::INTEGER);

        if (!tok)
        {
            error("Expected argument for instruction");
            return false;
        }

        return melSerializerWriteOp1_25(&this->codeSerializer, op, atoi(tok->value.c_str())) == 0;
    }

    bool compileTwoArgs(TVMOpcode op)
    {
        Token* tokA = consumeToken(MelTokenType::INTEGER);

        if (!tokA)
        {
            error("Expected first argument for instruction");
            return false;
        }

        Token* tokB = consumeToken(MelTokenType::INTEGER);

        if (!tokB)
        {
            error("Expected second argument for instruction");
            return false;
        }

        return melSerializerWriteOp2_13(
            &this->codeSerializer, 
            op, 
            atoi(tokA->value.c_str()),
            atoi(tokB->value.c_str())
        ) == 0;
    }

    bool compileNewCl(FunctionContext& fc)
    {
        Token* tokFunc = consumeToken(MelTokenType::SYMBOL);

        if (!tokFunc)
        {
            error("Expected a function name to call");
            return false;
        }

        TVMInstK protoId = 0;

        if (fc.funcLabel == tokFunc->value)
        {
            protoId = 0;
        }
        else if (fc.functionLabels.find(tokFunc->value) != fc.functionLabels.end())
        {
            protoId = fc.functionLabels[tokFunc->value] + 1;
        }
        else
        {
            error("Can't find function '" + tokFunc->value + "'");
            return false;
        }

        return melSerializerWriteOp1_25(&this->codeSerializer, MELON_OP_NEWCL, protoId) == 0;
    }

    bool compileCall(FunctionContext& fc, bool tail = false)
    {
        Token* tokArgs = consumeToken(MelTokenType::INTEGER);

        if (!tokArgs)
        {
            error("Expected the number of arguments with which the function will be called");
            return false;
        }

        Token* tokExpected = consumeToken(MelTokenType::INTEGER);

        if (!tokExpected)
        {
            error("Expected the number of expected return values from the function");
            return false;
        }

        return melSerializerWriteOp2_13(
            &this->codeSerializer, 
            tail ? MELON_OP_CALLTAIL : MELON_OP_CALL, 
            atoi(tokArgs->value.c_str()),
            atoi(tokExpected->value.c_str())
        ) == 0;
    }

    bool compileJmp(TInt32 offset)
    {
        return melSerializerWriteOp1_s25(&this->codeSerializer, MELON_OP_JMP, offset) == 0;
    }

    bool addJmp(FunctionContext& fc)
    {
        Token* tok = consumeToken(MelTokenType::SYMBOL);

        if (!tok)
        {
            error("Expected symbol token for the jump label");
            return false;
        }

        JmpEntry je;
        je.label = tok->value;
        je.offset = this->curBuffer;
        fc.jmpTable.push_back(je);

        this->curBuffer += sizeof(TVMInst);

        return true;
    }

    bool consumeUpvalues(FunctionContext& fc)
    {
        if (peekToken()->type != MelTokenType::PLUS)
        {
            return true;
        }

        consumeToken(MelTokenType::PLUS);
        Token* tok = consumeToken(MelTokenType::SYMBOL);

        if (!tok || tok->value != "upvalue")
        {
            error("Expected 'upvalue' keyword");
            return false;
        }

        tok = consumeToken(MelTokenType::SYMBOL);

        if (!tok || (tok->value != "stack" && tok->value != "parent"))
        {
            error("Expected 'stack' or 'parent' keyword to indicate the upvalue location");
            return false;
        }

        UpvalueInfo ui;
        ui.instack = tok->value == "stack" ? 1 : 0;

        tok = consumeToken(MelTokenType::INTEGER);

        if (!tok)
        {
            error("Expected integer to indicate the upvalue index in the parent's upvalues or in the stack");
            return false;
        }

        ui.idx = atoi(tok->value.c_str());

        fc.upvalues.push_back(ui);

        return true;
    }

    bool consumeFunctionSection(FunctionContext& parentFc)
    {
        consumeToken(MelTokenType::AT);
        Token* tok = consumeToken(MelTokenType::SYMBOL);

        if (!tok || tok->value != "function")
        {
            error("Expected @function section");
            return false;
        }

        tok = consumeToken(MelTokenType::SYMBOL);
        std::string funcLabel = tok->value;

        FunctionContext& fc = parentFc.addNewFunction(&this->vm, funcLabel);
        fc.funcLabel = funcLabel;
        
        tok = consumeToken(MelTokenType::INTEGER);
        TInteger args = atoi(tok->value.c_str());

        tok = consumeToken(MelTokenType::INTEGER);
        TInteger localsSlots = atoi(tok->value.c_str());

        Function* fn = melM_functionFromObj(fc.func);
        fn->localsSlots = localsSlots;
        fn->args = args;
        fn->debug.count = 0;
        fn->debug.lines = nullptr;

        while (
            peekToken() && 
            peekToken()->type == MelTokenType::PLUS &&
            peekToken(1)->type == MelTokenType::SYMBOL &&
            peekToken(1)->value == "upvalue"
        )
        {
            if (!consumeUpvalues(fc))
            {
                return false;
            }
        }

        if (!consumeFunctionBody(fc))
        {
            return false;
        }

        consumeToken(MelTokenType::AT);
        Token* endTok = consumeToken(MelTokenType::SYMBOL);

        if (!endTok || endTok->value != "end")
        {
            error("Missing function terminator @end");
            return false;
        }

        fn->upvaluesInfos.count = fc.upvalues.size();
        fn->upvaluesInfos.infos = reinterpret_cast<UpvalueInfo*>(malloc(sizeof(UpvalueInfo) * fc.upvalues.size()));
        assert(fn->upvaluesInfos.infos);

        for (size_t i = 0; i < fn->upvaluesInfos.count; i++)
        {
            fn->upvaluesInfos.infos[i] = fc.upvalues[i];
        }

        return true;
    }

    bool consumeFunctionBody(FunctionContext& fc, bool isMain = false)
    {
        while (
            peekToken() && 
            peekToken()->type == MelTokenType::AT &&
            peekToken(1)->type == MelTokenType::SYMBOL &&
            peekToken(1)->value == "function"
        )
        {
            if (!consumeFunctionSection(fc))
            {
                return false;
            }
        }

        Function* fn = melM_functionFromObj(fc.func);

        fn->funcPrototype.count = fc.functions.size();
        fn->funcPrototype.prototypes = reinterpret_cast<GCItem**>(malloc(fc.functions.size() * sizeof(GCItem*)));

        for (TSize i = 0; i < fc.functions.size(); i++)
        {
            fn->funcPrototype.prototypes[i] = fc.functions[i].func;
        }

        // @TODO: Quick and dirty, good ol' copy & paste

        if (
            peekToken() && 
            peekToken()->type == MelTokenType::AT &&
            peekToken(1) &&
            peekToken(1)->type == MelTokenType::SYMBOL &&
            peekToken(1)->value == "main"
        )
        {
            consumeToken(MelTokenType::AT);
            consumeToken(MelTokenType::SYMBOL);

            Token* tok = consumeToken(MelTokenType::INTEGER);
            TInteger localsSlots = atoi(tok->value.c_str());

            melM_functionFromObj(this->main.func)->localsSlots = localsSlots;
        }

        this->curBuffer = 0;
        this->buffer.clear();

        if (!consumeDataSection(fc))
        {
            return false;
        }
        
        TSize curConst = 0;

        fn->constants.count = fc.constants.size();
        fn->constants.constants = reinterpret_cast<Value*>(malloc(fc.constants.size() * sizeof(Value)));

        for (auto& c : fc.constants)
        {
            Value* v = &fn->constants.constants[curConst++];

            switch(c.value.type)
            {
                case MelTokenType::STRING:
                    v->type = MELON_TYPE_STRING;
                    v->pack.obj = melNewString(&this->vm, c.value.value.c_str(), c.value.value.size());
                    break;

                case MelTokenType::NUMBER:
                    v->type = MELON_TYPE_NUMBER;
                    v->pack.value.number = strtonum(c.value.value.c_str());
                    break;

                case MelTokenType::INTEGER:
                    v->type = MELON_TYPE_INTEGER;
                    v->pack.value.integer = atoi(c.value.value.c_str());
                    break;

                default:
                    error("Invaid constant token type. (Internal error)");
                    return false;
            }
        }

        if (!consumeTextSection(fc))
        {
            return false;
        }

        if (isMain)
        {
            melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_HALT);
        }

        if (!resolveJmps(fc))
        {
            return false;
        }

        fn->name = melNewString(&this->vm, fc.funcLabel.c_str(), fc.funcLabel.size());
        fn->native = nullptr;
        fn->size = this->curBuffer;
        fn->code = reinterpret_cast<Instruction*>(malloc(fn->size));
        memcpy(fn->code, &this->buffer[0], fn->size);

        return true;
    }

    bool consumeTextSection(FunctionContext& fc)
    {
        consumeToken(MelTokenType::DOT);
        Token* tok = consumeToken(MelTokenType::SYMBOL);

        if (!tok || tok->value != "text")
        {
            error("Expected .text section");
            return false;
        }

        while(peekToken())
        {
            if (peekToken()->type == MelTokenType::COLON)
            {                
                consumeToken(MelTokenType::COLON);
                Token* tok = consumeToken(MelTokenType::SYMBOL);
                fc.labels[tok->value] = this->curBuffer;
                continue;
            }

            if (peekToken()->type != MelTokenType::SYMBOL)
            {
                break;
            }

            Token* tok = consumeToken(MelTokenType::SYMBOL);

            if (tok->value == "NOP")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_NOP);
            }
            if (tok->value == "HALT")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_HALT);
            }
            else if (tok->value == "ADD")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_ADD);
            }
            else if (tok->value == "SUB")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_SUB);
            }
            else if (tok->value == "MUL")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_MUL);
            }
            else if (tok->value == "DIV")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_DIV);
            }
            else if (tok->value == "PUSHK")
            {
                compilePushK(fc);
            }
            else if (tok->value == "PUSHI")
            {
                compileSingleSignedArg(MELON_OP_PUSHI);
            }
            else if (tok->value == "PUSHNULL")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_PUSHNULL);
            }
            else if (tok->value == "PUSHTRUE")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_PUSHTRUE);
            }
            else if (tok->value == "PUSHFALSE")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_PUSHFALSE);
            }
            else if (tok->value == "TESTTRUE")
            {
                compileSingleArg(MELON_OP_TESTTRUE);
            }
            else if (tok->value == "TESTFALSE")
            {
                compileSingleArg(MELON_OP_TESTFALSE);
            }
            else if (tok->value == "TESTNULL")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_TESTNULL);
            }
            else if (tok->value == "EQ")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_EQ);
            }
            else if (tok->value == "GT")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_GT);
            }
            else if (tok->value == "LT")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_LT);
            }
            else if (tok->value == "LTE")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_LTE);
            }
            else if (tok->value == "GTE")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_GTE);
            }
            else if (tok->value == "NEWOBJ")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_NEWOBJ);
            }
            else if (tok->value == "SETOBJ")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_SETOBJ);
            }
            else if (tok->value == "GETOBJ")
            {
                compileSingleArg(MELON_OP_GETOBJ);
            }
            else if (tok->value == "PUSHLOC")
            {
                compileSingleArg(MELON_OP_PUSHLOC);
            }
            else if (tok->value == "STORELOC")
            {
                compileSingleArg(MELON_OP_STORELOC);
            }
            else if (tok->value == "POP")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_POP);
            }
            else if (tok->value == "PUSH")
            {
                compileSingleArg(MELON_OP_PUSH);
            }
            else if (tok->value == "JMP")
            {
                addJmp(fc);
            }
            else if (tok->value == "CALL")
            {
                compileCall(fc);
            }
            else if (tok->value == "NEWCL")
            {
                compileNewCl(fc);
            }
            else if (tok->value == "RETURN")
            {
                compileSingleArg(MELON_OP_RET);
            }
            else if (tok->value == "CALLTAIL")
            {
                compileCall(fc, true);
            }
            else if (tok->value == "DUMPINFO")
            {
                compileSingleArg(MELON_OP_DUMPINFO);
            }
            else if (tok->value == "PUSHUPVAL")
            {
                compileSingleArg(MELON_OP_PUSHUPVAL);
            }
            else if (tok->value == "STOREUPVAL")
            {
                compileSingleArg(MELON_OP_STOREUPVAL);
            }
            else if (tok->value == "CONCAT")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_CONCAT);
            }
            else if (tok->value == "GC")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_GC);
            }
            else if (tok->value == "NEWARR")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_NEWARR);
            }
            else if (tok->value == "GETARR")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_GETARR);
            }
            else if (tok->value == "GETIARR")
            {
                compileSingleArg(MELON_OP_GETIARR);
            }
            else if (tok->value == "SETARR")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_SETARR);
            }
            else if (tok->value == "SETIARR")
            {
                compileSingleArg(MELON_OP_SETIARR);
            }
            else if (tok->value == "PUSHARR")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_PUSHARR);
            }
            else if (tok->value == "SIZEARR")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_SIZEARR);
            }
            else if (tok->value == "OR")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_OR);
            }
            else if (tok->value == "AND")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_AND);
            }
            else if (tok->value == "NOT")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_NOT);
            }
            else if (tok->value == "SETGLOBAL")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_SETGLOBAL);
            }
            else if (tok->value == "GETGLOBAL")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_GETGLOBAL);
            }
            else if (tok->value == "SLICE")
            {
                compileSingleArg(MELON_OP_SLICE);
            }
            else if (tok->value == "BLESS")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_BLESS);
            }
            else if (tok->value == "PUSHTHIS")
            {
                compileSingleArg(MELON_OP_PUSHTHIS);
            }
            else if (tok->value == "NEWSYMBOL")
            {
                compileSingleArg(MELON_OP_NEWSYMBOL);
            }
            else if (tok->value == "PUSHBLESSING")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_PUSHBLESSING);
            }
            else if (tok->value == "PUSHITER")
            {
                compileSingleArg(MELON_OP_PUSHITER);
            }
            else if (tok->value == "NEXTITER")
            {
                compileSingleArg(MELON_OP_NEXTITER);
            }
            else if (tok->value == "BITLSHIFT")
            {
                compileSingleArg(MELON_OP_BITLSHIFT);
            }
            else if (tok->value == "BITRSHIFT")
            {
                compileSingleArg(MELON_OP_BITRSHIFT);
            }
            else if (tok->value == "BITAND")
            {
                compileSingleArg(MELON_OP_BITAND);
            }
            else if (tok->value == "BITOR")
            {
                compileSingleArg(MELON_OP_BITOR);
            }
            else if (tok->value == "BITNOT")
            {
                compileSingleArg(MELON_OP_BITNOT);
            }
            else if (tok->value == "BITXOR")
            {
                compileSingleArg(MELON_OP_BITXOR);
            }
            else if (tok->value == "SETUPFOR")
            {
                compileTwoArgs(MELON_OP_SETUPFOR);
            }
            else if (tok->value == "POW")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_POW);
            }
            else if (tok->value == "MOD")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_MOD);
            }
            else if (tok->value == "STORETOPCL")
            {
                compileSingleArg(MELON_OP_STORETOPCL);
            }
            else if (tok->value == "COALESCE")
            {
                melSerializerWriteOpVoid(&this->codeSerializer, MELON_OP_COALESCE);
            }
            else
            {
                error("Invalid instruction: " + tok->value);
            }
        }

        return true;
    }

    bool resolveJmps(FunctionContext& fc)
    {
        TSize oldCur = this->curBuffer;

        for (const auto& j : fc.jmpTable)
        {
            if (fc.labels.find(j.label) == fc.labels.end())
            {
                error("Can't find label '" + j.label + "'");
                return false;
            }

            TInt64 jmpAddr = fc.labels[j.label];
            TInt64 jmpOffset = (jmpAddr - j.offset) - sizeof(TVMInst);

            if (jmpOffset % sizeof(TVMInst) != 0)
            {
                error("Unaligned jump performed for label '" + j.label + "'");
                return false;
            }

            this->curBuffer = j.offset;
            compileJmp(jmpOffset / sizeof(TVMInst));
        }

        this->curBuffer = oldCur;

        return true;
    }

    bool assemble()
    {
        this->main.func = melNewFunction(&this->vm);
        melM_functionFromObj(this->main.func)->args = 0;
        this->main.funcLabel = "main";

        return consumeFunctionBody(this->main, true);
    }

    bool write()
    {
        melSerializerWriteHeader(&this->serializer, &this->header);
        melSerializerWriteObj(&this->serializer, this->main.func);

        return true;
    }

private:
    void error(const std::string& err)
    {
        std::cerr << "[ERROR] line: " << this->line << " " << err << std::endl;
    }

private:
    uint64_t line = 0;
    uint32_t col = 0;
    std::string fileName = "";
    std::string outFile = "";
    ProgramHeader header;
    std::vector<Token> tokens = {};
    FunctionContext main = {};

    size_t curToken = 0;

    Serializer serializer;
    Serializer codeSerializer;

    std::vector<TByte> buffer;
    TSize curBuffer = 0; 

    std::ofstream outFileStr;

    VM vm;
};

} // namespace melon

int main(int argc, char** argv)
{
    cxxopts::Options options("Melon Assembler", "Melon VM bytecode assembler");

    options.add_options()
        ("f,file", "Input file to assemble", cxxopts::value<std::string>())
        ("o,outfile", "Output file where the assembled bytecode will be written", cxxopts::value<std::string>()->default_value("out.mbc"))
    ;

    auto result = options.parse(argc, argv);

    if (result.count("f") <= 0)
    {
        std::cerr << "Please specify an input file to assemble with the -f option." << std::endl;
        return 1;
    }

    melon::Assembler ass(result["f"].as<std::string>(), result["o"].as<std::string>());

    if (!ass.tokenize())
    {
        std::cerr << "Tokenization error." << std::endl;
        return 1;
    }

    if (!ass.assemble())
    {
        std::cerr << "Assembler error." << std::endl;
        return 1;
    }

    if (!ass.write())
    {
        std::cerr << "Can't write output file." << std::endl;
        return 1;
    }

    return 0;
}