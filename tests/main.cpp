#define CATCH_CONFIG_MAIN
#include "melon/libs/catch2/single_include/catch2/catch.hpp"
#include <dirent.h>
#include <fstream>

extern "C" {
#include "melon/melon.h"
#include "melon/core/tstring.h"
#include "melon/core/stack.h"
#include "melon/core/object.h"
}

namespace {

template<typename T>
void valSetType(T v, Value* val);

template<>
void valSetType(TInteger v, Value* val)
{
    val->type = MELON_TYPE_INTEGER;
    val->pack.value.integer = v;
}

template<>
void valSetType(TNumber v, Value* val)
{
    val->type = MELON_TYPE_NUMBER;
    val->pack.value.number = v;
}

template<typename T>
void valCheckType(T v, Value* val);

template<>
void valCheckType(TInteger v, Value* val)
{
    REQUIRE(val->type == MELON_TYPE_INTEGER);
    REQUIRE(val->pack.value.integer == v);
}

template<>
void valCheckType(TNumber v, Value* val)
{
    REQUIRE(val->type == MELON_TYPE_NUMBER);
    REQUIRE(val->pack.value.number == v);
}

template<>
void valCheckType(std::string b, Value* val)
{
    REQUIRE(val->type == MELON_TYPE_STRING);
    REQUIRE(val->pack.obj);
    
    String* strObj = melM_strFromObj(val->pack.obj);
    char* str = melM_strDataFromObj(val->pack.obj);

    std::string a(str, strObj->len);
    REQUIRE(a == b);
}

template <typename T>
void push(VM& vm, T x)
{
    Value val;
    valSetType<T>(x, &val);
    melM_stackPush(&vm.stack, &val);
}

template <typename T>
void check(VM& vm, T expected)
{
    StackEntry* se = melM_stackOffset(&vm.stack, 0);
    REQUIRE(se != NULL);
    valCheckType<T>(expected, se);
}

}

namespace {
    TByte* readFile(const std::string& path, size_t* size, bool binary = false)
    {
        std::ifstream f;
        f.open(path, std::ios_base::in | (binary ? std::ios_base::binary : 0));
        REQUIRE(f.is_open());

        f.seekg(0, std::ios_base::seekdir::end);
        *size = f.tellg();
        f.seekg(0, std::ios_base::seekdir::beg);

        TByte* prog = new TByte[*size];
        f.read(reinterpret_cast<char*>(prog), *size);

        return prog;
    }

    void dumpCompiled(std::vector<TByte>& outData, Compiler& c)
    {
        Serializer s;

        s.ctx = &outData;
        s.error = [] (void* ctx, const char* message) {
            printf("Serialization error: %s\n", message);
            assert(0);
        };
        
        s.writer = [] (void* ctx, const void* data, TSize num) -> TRet {
            std::vector<TByte>& outData = *static_cast<std::vector<TByte>*>(ctx);
            const TByte* inData = static_cast<const TByte*>(data);

            std::vector<TByte>::size_type start = outData.size();
            outData.resize(outData.size() + num);
            memcpy(&outData[start], inData, num);

            return 0;
        };

        ProgramHeader header;
        melSerializerWriteHeader(&s, &header);
        melSerializerWriteObj(&s, c.main.func);
    }

    std::string readTextFile(const std::string& path)
    {
        size_t size = 0;
        TByte* content = readFile(path, &size);
        
        std::string res(reinterpret_cast<char*>(content), size);

        delete[] content;

        return std::move(res);
    }

    void runByteCodeBufferTest(VM* vm, TByte* prog, size_t progSize, const std::string& path)
    {
        std::string expectedRes = readTextFile(path + ".out");
        std::string output = "";

        VM vmNew;

        if (!vm)
        {
            VMConfig config = {};
            REQUIRE(melCreateVM(&vmNew, &config) == 0);
            vm = &vmNew;
        }

        vm->config.printCtx = static_cast<void*>(&output);

        vm->config.printFn = [] (struct StrFormat* sf, void* ctx) {
            std::string newPiece(sf->buffer, sf->used);
            std::string* outStr = static_cast<std::string*>(ctx);
            *outStr += newPiece;
        };

        GCItem* program = NULL;

        REQUIRE(melLoadProgramVM(vm, prog, progSize, &program, path.c_str(), nullptr) == 0);

        REQUIRE(melRunProgramVM(vm, program) == 0);
        melRemoveRootGC(vm, &vm->gc, program);
        REQUIRE(expectedRes == output);
    }

    void runByteCodeTest(const std::string& path)
    {
        SECTION(path)
        {
            size_t size = 0;
            TByte* prog = readFile(path + ".mbc", &size, true);
            runByteCodeBufferTest(nullptr, prog, size, path);
            delete[] prog;
        }
    }

    void runScriptStringTest(const std::string& source, const std::string& testFile)
    {
        VMConfig conf = {};
        VM vm;

        REQUIRE(melCreateVM(&vm, &conf) == 0);

        Compiler c;
        REQUIRE(melCreateCompiler(&c, &vm, source.c_str(), source.size()) == 0);

        c.lexer.error = [] (void* ctx, const char* message, const char* file, TSize line, TSize column) {
            printf("Parsing error at %s %llu:%llu - %s\n", file ? file : "Unknown file", line, column, message);
            assert(0);
        };

        c.error = [] (void* ctx, const char* message, const char* file, TSize line, TSize column) {
            printf("Compilation error at %s %llu:%llu - %s\n", file ? file : "Unknown file", line, column, message);
            assert(0);
        };

        REQUIRE(melRunCompiler(&c) == 0);

        std::vector<TByte> data;
        dumpCompiled(data, c);

        runByteCodeBufferTest(&vm, &data[0], data.size(), testFile);     
    }

    void runScriptTest(const std::string& path)
    {
        SECTION(path)
        {
            std::string source = readTextFile(path + ".ms");
            runScriptStringTest(source, path);
        }
    }
    
    void runDirTests(const std::string& path) 
    {
        DIR* dir = opendir(path.c_str());
        struct dirent* ent = nullptr;

        REQUIRE(dir);

        while ((ent = readdir (dir))) 
        {
            std::string entName(ent->d_name);

            if (entName == "." || entName == "..")
            {
                continue;
            }

            if (entName.substr(entName.size() - 4, 4) == ".mbc")
            {
                std::string filePath = path;
                
                if (path[filePath.size() - 1] != '/')
                {
                    filePath += "/";
                }

                filePath += entName.substr(0, entName.size() - 4);

                runByteCodeTest(filePath);
            }
            else if (entName.substr(entName.size() - 3, 3) == ".ms")
            {
                 std::string filePath = path;
                
                if (path[filePath.size() - 1] != '/')
                {
                    filePath += "/";
                }

                filePath += entName.substr(0, entName.size() - 3);

                runScriptTest(filePath);
            }
        }

        closedir(dir);
    }

    void assertToken(const std::string& source, TokenType type, uint32_t len)
    {
        Lexer lexer;
        REQUIRE(melCreateLexer(&lexer, source.c_str(), source.size()) == 0);
        
        if (type == MELON_TYPE_NONE)
        {
            lexer.error = [] (void* ctx, const char* message, const char* file, TSize line, TSize column) { };
        }
        else
        {
            lexer.error = [] (void* ctx, const char* message, const char* file, TSize line, TSize column) { 
                FAIL("Lexer error");
            };
        }

        REQUIRE(melAdvanceLexer(&lexer) == 0);
        REQUIRE(melAdvanceLexer(&lexer) == 0);

        REQUIRE(melCurTokenLexer(&lexer)->type == type);
        REQUIRE(melCurTokenLexer(&lexer)->len == len);
    }
}

TEST_CASE("Stack") {
    SECTION("Basic") {
        Stack stack;
        REQUIRE(melCreateStack(&stack, 1, sizeof(StackEntry)) == 0);

        stack.stackInvalidatedCtx = NULL;
        stack.stackInvalidatedFunc = [] (void* ctx, void* old) -> TRet { return 0; };

        for (int i = 0; i < 10; i++)
        {
            Value val;
            val.type = MELON_TYPE_INTEGER;
            val.pack.value.integer = i;

            melM_stackPush(&stack, &val);

            REQUIRE(stack.top == i + 1);
            REQUIRE(melM_stackOffset(&stack, 0) != NULL);
        }

        for (int i = 10; i > 0; i--)
        {
            REQUIRE(stack.top == i);
            StackEntry* entry = melM_stackPop(&stack);
            REQUIRE(entry != NULL);
            REQUIRE(entry->type == MELON_TYPE_INTEGER);
            REQUIRE(entry->pack.value.integer == i - 1);
        }

        REQUIRE(melM_stackIsEmpty(&stack));

        for (int i = 0; i <= 1024; i++)
        {
            Value val;
            val.type = MELON_TYPE_INTEGER;
            val.pack.value.integer = i;
            melM_stackPush(&stack, &val);
        }

        REQUIRE(stack.top == 1024 + 1);
        REQUIRE(stack.size > 1024);

        melM_stackSetTop(&stack, 0);
        REQUIRE(melM_stackIsEmpty(&stack));
        REQUIRE(stack.size > 1024);

        melResizeStack(&stack, 0);
        REQUIRE(melM_stackIsEmpty(&stack));
        REQUIRE(stack.size == 0);

        {
            Value val;
            val.type = MELON_TYPE_INTEGER;
            val.pack.value.integer = 1;
            melM_stackPush(&stack, &val);
        }

        REQUIRE(stack.top == 1);
        REQUIRE(stack.size == 2);
    }
}

TEST_CASE("Object") {
    SECTION("Basic") {
        VM vm;
        VMConfig config = {};
        REQUIRE(melCreateVM(&vm, &config) == 0);
        melSetPauseGC(&vm, &vm.gc, 1);

        GCItem* obj = melNewObject(&vm);

        Value key;
        key.type = MELON_TYPE_STRING;
        key.pack.obj = melNewString(&vm, "test", 4);
        REQUIRE(key.pack.obj != NULL);

        Value val;
        val.type = MELON_TYPE_INTEGER;
        val.pack.value.integer = 1024;

        REQUIRE(melSetValueObject(&vm, obj, &key, &val) == 0);

        Value* retVal = melGetValueObject(&vm, obj, &key);
        REQUIRE(retVal != NULL);
        REQUIRE(retVal->type == MELON_TYPE_INTEGER);
        REQUIRE(retVal->pack.value.integer == 1024);

        val.type = MELON_TYPE_NUMBER;
        val.pack.value.number = 0.5;

        REQUIRE(melSetValueObject(&vm, obj, &key, &val) == 0);

        retVal = melGetValueObject(&vm, obj, &key);
        REQUIRE(retVal != NULL);
        REQUIRE(retVal->type == MELON_TYPE_NUMBER);
        REQUIRE(retVal->pack.value.number == 0.5);

        REQUIRE(melRemoveKeyObject(&vm, obj, &key) == 0);

        retVal = melGetValueObject(&vm, obj, &key);
        REQUIRE(retVal == NULL);
    }
}

TEST_CASE("VM") {
    SECTION("Bytecode") {
        runDirTests("fixtures/vm/bytecode");
    }
}

TEST_CASE("Lexer") {
    SECTION("Basic") {
        std::string source = 
            "   if (     my_var1 == 145 ) { let _myVar2 = () => { return \"this is a test string\"; } }  "
            "\n let b = 123.4; if (b >= 0.2) { a.b = \"asdasd\"; } 6;"
        ;

        Lexer lexer;
        REQUIRE(melCreateLexer(&lexer, source.c_str(), source.size()) == 0);

        REQUIRE(melAdvanceLexer(&lexer) == 0);

        lexer.error = [] (void* ctx, const char* message, const char* file, TSize line, TSize column) {
            printf("Parsing error at %s %llu:%llu - %s\n", file ? file : "Unknown file", line, column, message);
            assert(0);
        };

        REQUIRE(melAdvanceLexer(&lexer) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_IF) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_LEFT_PAREN) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_NAME) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_EQEQ) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_INTEGER) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_RIGHT_PAREN) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_LEFT_BRACE) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_LET) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_NAME) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_EQ) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_LEFT_PAREN) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_RIGHT_PAREN) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_FAT_ARROW) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_LEFT_BRACE) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_RETURN) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_STRING) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_SEMICOLON) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_RIGHT_BRACE) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_RIGHT_BRACE) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_LET) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_NAME) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_EQ) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_NUMBER) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_SEMICOLON) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_IF) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_LEFT_PAREN) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_NAME) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_GTEQ) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_NUMBER) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_RIGHT_PAREN) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_LEFT_BRACE) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_NAME) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_DOT) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_NAME) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_EQ) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_STRING) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_SEMICOLON) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_RIGHT_BRACE) == 0);
        REQUIRE(melPopTypeLexer(&lexer, MELON_TOKEN_INTEGER) == 0);
    }

    SECTION("Numbers") {
        assertToken("5", MELON_TOKEN_INTEGER, 1);
        assertToken("12345", MELON_TOKEN_INTEGER, 5);
        assertToken("4.0", MELON_TOKEN_NUMBER, 3);
        assertToken("4.0.0", MELON_TOKEN_NUMBER, 3);
        assertToken("4..0.0", MELON_TOKEN_INTEGER, 1);
        assertToken("4e0", MELON_TOKEN_NUMBER, 3);
        assertToken("2e-0", MELON_TOKEN_NUMBER, 4);
        assertToken("2E+5", MELON_TOKEN_NUMBER, 4);
        assertToken("2E+", MELON_TOKEN_INTEGER, 1);
        assertToken("2+e4", MELON_TOKEN_INTEGER, 1);
        assertToken("2.53432e04", MELON_TOKEN_NUMBER, 10);
        assertToken("2e5.0", MELON_TOKEN_NUMBER, 3);
        assertToken("2.53432e-24", MELON_TOKEN_NUMBER, 11);
    }
}

TEST_CASE("Compiler") {
    SECTION("Language") {
        runDirTests("fixtures/vm/language");
    }
}