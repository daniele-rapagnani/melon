#include "melon/core/compiler.h"
#include "melon/core/tstring.h"
#include "melon/core/object.h"
#include "melon/core/utils.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define melM_error(c, str) \
    (c)->hasErrors = 1; \
    ((c)->error ? (c)->error( \
        (c)->errorCtx, \
        str, \
        (c)->lexer.file, \
        melCurTokenLexer(&((c)->lexer))->line, \
        melCurTokenLexer(&((c)->lexer))->column \
    ) : 0)

#ifdef _DEBUG_COMPILER
#define melM_log(...) printf(__VA_ARGS__)
#else
#define melM_log(...) do {} while(0)
#endif

#define melM_writeOp1_s25(c, op, v) \
    melUpdateSourceLine(c); \
    melSerializerWriteOp1_s25(&((c)->serializer), op, v); \
    melM_log("" MELON_PRINTF_SIZE ": %s %ld\n", melM_functionFromObj(c->curFunc->func)->debug.lines[c->curFunc->pc - 1], #op, (TInteger)(v))

#define melM_writeOp1_25(c, op, v) \
    melUpdateSourceLine(c); \
    melSerializerWriteOp1_25(&((c)->serializer), op, v); \
    melM_log("" MELON_PRINTF_SIZE ": %s " MELON_PRINTF_SIZE "\n", melM_functionFromObj(c->curFunc->func)->debug.lines[c->curFunc->pc - 1], #op, (TInteger)(v))

#define melM_writeOp2_13(c, op, a, b) \
    melUpdateSourceLine(c); \
    melSerializerWriteOp2_13(&((c)->serializer), op, a, b); \
    melM_log("" MELON_PRINTF_SIZE ": %s " MELON_PRINTF_SIZE " " MELON_PRINTF_SIZE "\n", melM_functionFromObj(c->curFunc->func)->debug.lines[c->curFunc->pc - 1], #op, (TInteger)(a), (TInteger)(b))

#define melM_writeOpVoid(c, op) \
    melUpdateSourceLine(c); \
    melSerializerWriteOpVoid(&((c)->serializer), op); \
    melM_log("" MELON_PRINTF_SIZE ": %s\n", melM_functionFromObj(c->curFunc->func)->debug.lines[c->curFunc->pc - 1], #op)

typedef void(*CompileCb)(Compiler* c);
typedef void(*CompileDeferedCb)(Compiler* c, const Token* t);
typedef TByte Precedence;

typedef enum ShortCircuitStrategy
{
    SCS_NONE = 0,
    SCS_BOOLEAN_EXIT_FALSE,
    SCS_BOOLEAN_EXIT_TRUE
} ShortCircuitStrategy;

struct Operator
{
    CompileCb unary;
    CompileDeferedCb infix;
    Precedence unaryPrecedence;
    Precedence infixPrecedence;
    ShortCircuitStrategy shortCircuit;
};

typedef enum LHSType
{
    LHS_NONE,
    LHS_LOCAL,
    LHS_UPVAL,
    LHS_GLOBAL,
    LHS_OBJ,
    LHS_ARRAY,
    LHS_ARRAYPUSH,
    LHS_ARRAYSLICE_COPY,
    LHS_ARRAYSLICE_START,
    LHS_ARRAYSLICE_END,
    LHS_ARRAYSLICE_START_END,
    LHS_THIS,
    LHS_METHOD_ACCESS,
    LHS_SUBEXPR,
    LHS_CALL
} LHSType;

typedef struct LHSNode
{
    LHSType type;
    TSize varIdx;
} LHSNode;

static void melCompileUnaryOp(Compiler* c);
static void melCompileBinaryOp(Compiler* c, const Token* t);
static void melCompileSuffixed(Compiler* c);

#define MAX_PRECEDENCE 13

/**
 * Operators precedence is inspired by C++ and ES6.
 *
 * References:
 * - https://en.cppreference.com/w/cpp/language/operator_precedence
 * - https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/Operator_Precedence
 */

static struct Operator operators[] = {
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_NONE = 0, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_LEFT_PAREN, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_RIGHT_PAREN, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_LEFT_BRACKET, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_RIGHT_BRACKET, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_LEFT_BRACE, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_RIGHT_BRACE, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_COLON, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_SEMICOLON, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_DOT, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_COMMA, */
    { melCompileUnaryOp, melCompileBinaryOp, 1, 2, SCS_NONE }, /* MELON_TOKEN_STAR, */
    { NULL, melCompileBinaryOp, 0, 2, SCS_NONE }, /* MELON_TOKEN_SLASH, */
    { NULL, melCompileBinaryOp, 0, 2, SCS_NONE }, /* MELON_TOKEN_PERCENT, */
    { NULL, melCompileBinaryOp, 0, 3, SCS_NONE }, /* MELON_TOKEN_PLUS, */
    { melCompileUnaryOp, melCompileBinaryOp, 1, 3, SCS_NONE }, /* MELON_TOKEN_MINUS, */
    { NULL, melCompileBinaryOp, 0, 9, SCS_NONE }, /* MELON_TOKEN_PIPE, */
    { NULL, melCompileBinaryOp, 0, 11, SCS_BOOLEAN_EXIT_TRUE }, /* MELON_TOKEN_PIPEPIPE, */
    { NULL, melCompileBinaryOp, 0, 7, SCS_NONE }, /* MELON_TOKEN_AMP, */
    { NULL, melCompileBinaryOp, 0, 10, SCS_BOOLEAN_EXIT_FALSE }, /* MELON_TOKEN_AMPAMP, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_QUESTION, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_EQ, */
    { NULL, melCompileBinaryOp, 0, 5, SCS_NONE }, /* MELON_TOKEN_LT, */
    { melCompileUnaryOp, melCompileBinaryOp, 1, 5, SCS_NONE }, /* MELON_TOKEN_GT, */
    { NULL, melCompileBinaryOp, 0, 5, SCS_NONE }, /* MELON_TOKEN_LTEQ, */
    { NULL, melCompileBinaryOp, 0, 5, SCS_NONE }, /* MELON_TOKEN_GTEQ, */
    { NULL, melCompileBinaryOp, 0, 6, SCS_NONE }, /* MELON_TOKEN_EQEQ, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_EQLT, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_FAT_ARROW, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_BREAK, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_ELSE, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_FALSE, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_FOR, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_IF, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_NULL, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_RETURN, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_THIS, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_TRUE, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_LET, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_WHILE, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_NAME, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_NUMBER, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_INTEGER, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_STRING, */
    { melCompileUnaryOp, NULL, 1, 0, SCS_NONE }, /* MELON_TOKEN_EXCLAMATION, */
    { melCompileUnaryOp, NULL, 1, 0, SCS_NONE }, /* MELON_TOKEN_HASH, */
    { NULL, melCompileBinaryOp, 0, 6, SCS_NONE }, /* MELON_TOKEN_DOTDOT, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_FUNC, */
    { melCompileUnaryOp, melCompileBinaryOp, 1, 5, SCS_NONE }, /* MELON_TOKEN_AT, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_RIGHT_ARROW, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_DOLLAR, */
    { NULL, melCompileBinaryOp, 0, 2, SCS_NONE }, /* MELON_TOKEN_NOTEQ, */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_IN */
    { NULL, melCompileBinaryOp, 0, 4, SCS_NONE }, /* MELON_TOKEN_GTGT */
    { NULL, melCompileBinaryOp, 0, 4, SCS_NONE }, /* MELON_TOKEN_LTLT */
    { melCompileUnaryOp, melCompileBinaryOp, 1, 8, SCS_NONE }, /* MELON_TOKEN_CARET */
    { NULL, melCompileBinaryOp, 0, 3, SCS_NONE }, /* MELON_TOKEN_CARETCARET */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_CONTINUE */
    { NULL, melCompileBinaryOp, 0, 12, SCS_NONE }, /* MELON_TOKEN_QUESTIONQUESTION */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_ELLIPSIS */
    { NULL, NULL, 0, 0, SCS_NONE }, /* MELON_TOKEN_EOF */
};

static TRet melUpdateSourceLine(Compiler* c);
static TRet melMarkInstLine(Compiler* c);
static TRet melCompileExpression(Compiler* c, Precedence precedence);
static TRet melCompileStatement(Compiler* c);
static TRet melCreateFunctionDef(Compiler* c, FunctionDef* funcDef);
static TRet melCompileFunction(Compiler* c, FunctionDef* funcDef);
static TRet melCompileLocalDeclaration(Compiler* c, TSize* varIdx);
static TRet melNewBlock(Compiler* c, TSize* blockTop);
static TRet melEndBlock(Compiler* c, TSize oldBlockTop);
static TRet melAddNamedFunctionLocal(
    Compiler* c, 
    FunctionDef* namedFunc, 
    TSize funcId, 
    FunctionDef* dstFunc
);
static TRet patchJump(Compiler* c, TSize jmpLocation, TSize jmpDestination);

typedef enum ResolvedVariableType
{
    RVT_NONE   = 0, /* Same as LHSType */
    RVT_LOCAL  = 1,
    RVT_UPVAL  = 2,
    RVT_GLOBAL = 3
} ResolvedVariableType;

typedef struct ResolvedVariable 
{
    ResolvedVariableType type;
    TSize idx;
    FunctionDef* fn;
} ResolvedVariable;

static LoopInfo* melPushLoop(Compiler* c, TSize startAddr)
{
    LoopInfo* li = melM_stackAllocRaw(&c->loopStack);
    li->loopStart = startAddr;
    li->breaks = NULL;

    return li;
}

static void melCloseInnerLoop(Compiler* c)
{
    LoopInfo* li = melM_stackPop(&c->loopStack);
    LoopBreak* lb = li->breaks;
    LoopBreak* tmp = NULL;

    while (lb != NULL)
    {
        patchJump(c, lb->patchOffset, lb->cont ? li->loopStart : li->loopEnd);
        tmp = lb;
        lb = lb->next;

        free(tmp);
    }
}

static LoopBreak* melAddBreak(Compiler* c)
{
    LoopInfo* li = melM_stackOffset(&c->loopStack, 0);

    if (li == NULL)
    {
        return NULL;
    }

    LoopBreak* lb = malloc(sizeof(LoopBreak));
    lb->patchOffset = c->curFunc->buffer.size;
    lb->next = li->breaks;
    li->breaks = lb;

    return lb;
}

static TRet melOptimizeShortCircuits(Compiler* c)
{
    if (c->shortCircuitStack.top > 1)
    {
        for (TSize i = c->shortCircuitStack.top - 1; i > 0; i--)
        {
            ShortCircuitInfo* a = melM_stackGet(&c->shortCircuitStack, i - 1);
            ShortCircuitInfo* b = melM_stackGet(&c->shortCircuitStack, i);

            if ((a->jmpDest - 1) >= b->checkStartOff && (a->jmpDest - 1) <= b->jmpOff && a->testTrue == b->testTrue)
            {
                a->jmpDest = b->jmpDest;
                patchJump(c, a->jmpOff, a->jmpDest - 1);
            }
        }
    }

    c->shortCircuitStack.top = 0;   

    return 0;
}

static ShortCircuitInfo* melCompileBooleanShortCircuit(Compiler* c, ShortCircuitStrategy s)
{
    assert(s != SCS_NONE);

    melM_stackEnsure(&c->shortCircuitStack, c->shortCircuitStack.top + 1);
    ShortCircuitInfo* sci = melM_stackAllocRaw(&c->shortCircuitStack);

    sci->checkStartOff = c->curFunc->buffer.size;
    sci->testTrue = s == SCS_BOOLEAN_EXIT_TRUE;
    melM_writeOp1_25(c, sci->testTrue ? MELON_OP_TESTFALSE : MELON_OP_TESTTRUE, 0);
    melM_writeOp1_s25(c, MELON_OP_JMP, 0);
    sci->jmpOff = c->curFunc->buffer.size - sizeof(TVMInst);
    sci->jmpDest = 0;

    return sci;
}

static TRet melFinalizeShortCircuit(Compiler* c, ShortCircuitInfo* sci)
{
    sci->jmpDest = c->curFunc->buffer.size + 1;
    return 0;
}

static TRet melFindLocal(Compiler* c, FunctionDef* fd, const char* name, TSize length, TSize* out)
{
    if (fd->usedVariables == 0)
    {
        return 1;
    }

    for (TSize i = fd->usedVariables; i > 0; i--)
    {
        const VariableInfo* cur = &fd->variables[i - 1];

        if (cur->isDeleted)
        {
            continue;
        }

        if (
            cur->len == length
            && memcmp(name, cur->name, length) == 0
        )
        {
            if (out != NULL)
            {
                *out = i - 1;
            }

            return 0;
        }
    }

    return 1;
}

static TRet melAddUpvalue(
    Compiler* c, 
    FunctionDef* fd, 
    TSize index, 
    TBool isLocal,
    const char* name,
    TSize len,
    TSize* out
)
{
    for (TSize i = 0; i < fd->usedUpvalues; i++)
    {
        if (fd->upvalues[i].index == index && fd->upvalues[i].isLocal == isLocal)
        {
            if (out != NULL)
            {
                *out = i;
            }

            return 0;
        }
    }

    fd->upvalues[fd->usedUpvalues].index = index;
    fd->upvalues[fd->usedUpvalues].isLocal = isLocal;
    fd->upvalues[fd->usedUpvalues].name = name;
    fd->upvalues[fd->usedUpvalues].len = len;
    *out = fd->usedUpvalues;
    fd->usedUpvalues++;

    return 0;
}

static TRet melFindUpvalue(Compiler* c, FunctionDef* fd, const char* name, TSize len, TSize* out)
{
    for (TSize i = 0; i < fd->usedUpvalues; i++)
    {
        if (
            fd->upvalues[i].len == len
            && memcmp(fd->upvalues[i].name, name, len) == 0
        )
        {
            *out = i;
            return 0;
        }
    }

    return 1;
}

static TRet melResolveVariable(
    Compiler* c, 
    FunctionDef* fd, 
    const char* name, 
    TSize len,
    ResolvedVariable* var
)
{
    if (fd == NULL)
    {
        var->fn = NULL;
        var->type = RVT_GLOBAL;

        return 0;
    }

    TSize idx = 0;

    if (melFindLocal(c, fd, name, len, &idx) == 0)
    {
        if (fd != c->curFunc)
        {
            fd->variables[idx].isUpvalue = 1;
        }

        var->fn = fd;
        var->idx = idx;
        var->type = RVT_LOCAL;

        return 0;
    }

    if (melFindUpvalue(c, fd, name, len, &idx) == 0)
    {
        var->fn = fd;
        var->type = RVT_UPVAL;
        var->idx = idx;

        return 0;
    }

    melResolveVariable(c, fd->parent, name, len, var);

    if (var->type == RVT_LOCAL || var->type == RVT_UPVAL)
    {
        if (melAddUpvalue(c, fd, var->idx, var->type == RVT_LOCAL, name, len, &idx) != 0)
        {
            assert(0);
            return 1;
        }

        var->type = RVT_UPVAL;
        var->idx = idx;
        var->fn = fd;

        return 0;
    }

    // If we are here this is a global
    return 0;
}

static void pushConstant(Compiler* c, Value* val)
{
    Function* fn = melM_functionFromObj(c->curFunc->func);
    StaticConstants* sc = &fn->constants;
    melGrowBuffer((void**)&sc->constants, &sc->reserved, sizeof(Value), sc->count + 1);
    sc->constants[sc->count] = *val;
    sc->count++;

    melM_writeOp1_25(c, MELON_OP_PUSHK, sc->count - 1);
}

static TRet findStringConstant(Compiler* c, GCItem* str, TSize* index)
{
    //@TODO: A hashtable is probably the best thing to use
    //       but considering that we can compile to bytecode
    //       I'm not spending too much time on the compiler performance.

    Function* fn = melM_functionFromObj(c->curFunc->func);
    StaticConstants* sc = &fn->constants;

    for (TSize i = 0; i < sc->count; i++)
    {
        if (sc->constants[i].type != MELON_TYPE_STRING)
        {
            continue;
        }

        if (melCmpStrings(c->vm, str, sc->constants[i].pack.obj) == 0)
        {
            *index = i;
            return 0;
        }
    }

    return 1;
}

static void pushStringConstant(Compiler* c, const char* str, TSize len)
{
    GCItem* string = melNewString(c->vm, str, len);
    assert(string != NULL);

    String* strObj = melM_strFromObj(string);
    
    if (strObj->internalized)
    {
        TSize constId;

        // @TODO: Problem is we are not sure that the string was
        //        internalized because it is a constant in this function.
        //        This is a bit of a hack.

        if (findStringConstant(c, string, &constId) == 0)
        {
            melM_writeOp1_25(c, MELON_OP_PUSHK, constId);
            return;
        }
    }

    Value val;
    val.type = MELON_TYPE_STRING;
    val.pack.obj = string;

    pushConstant(c, &val);
}

static void pushStringConstantToken(Compiler* c, const Token* t)
{
    if (t->type == MELON_TOKEN_STRING)
    {
        pushStringConstant(c, (const char*)t->buffer.buffer, t->buffer.size - 1);
    }
    else
    {
        pushStringConstant(c, t->start, t->len);
    }
}

static void pushNumberConstant(Compiler* c, TNumber value)
{
    Value val;
    val.type = MELON_TYPE_NUMBER;
    val.pack.value.number = value;

    pushConstant(c, &val);
}

static void pushIntegerConstant(Compiler* c, TInteger value)
{
    Value val;
    val.type = MELON_TYPE_INTEGER;
    val.pack.value.integer = value;

    pushConstant(c, &val);
}

static TRet melCompileRootExpression(Compiler* c)
{
    if (melCompileExpression(c, MAX_PRECEDENCE) != 0)
    {
        return 1;
    }

    return melOptimizeShortCircuits(c);
}

static TRet melCompileObjectDefinition(Compiler* c)
{   
    melMarkInstLine(c);

    melPopTypeLexer(&c->lexer, MELON_TOKEN_LEFT_BRACE);
    melM_writeOpVoid(c, MELON_OP_NEWOBJ);

    while(
        melCurTokenLexer(&c->lexer)->type == MELON_TOKEN_NAME
        || melCurTokenLexer(&c->lexer)->type == MELON_TOKEN_STRING
        || melCurTokenLexer(&c->lexer)->type == MELON_TOKEN_LEFT_BRACKET
    )
    {
        melM_writeOp1_25(c, MELON_OP_PUSH, 0);

        if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_LEFT_BRACKET) == 0)
        {
            melCompileRootExpression(c);
            melPopTypeLexer(&c->lexer, MELON_TOKEN_RIGHT_BRACKET);
        }
        else
        {
            const Token* t = melCurTokenLexer(&c->lexer);
            pushStringConstantToken(c, t);
            melAdvanceLexer(&c->lexer);    
        }

        melPopTypeLexer(&c->lexer, MELON_TOKEN_EQ);

        melCompileRootExpression(c);

        melM_writeOpVoid(c, MELON_OP_SETOBJ);
        
        if (melCurTokenLexer(&c->lexer)->type == MELON_TOKEN_RIGHT_BRACE)
        {
            break;
        }

        melPopTypeLexer(&c->lexer, MELON_TOKEN_COMMA);
    }

    melPopTypeLexer(&c->lexer, MELON_TOKEN_RIGHT_BRACE);

    return 0;
}

static TRet melCompileArrayDefinition(Compiler* c)
{   
    melMarkInstLine(c);
    
    melPopTypeLexer(&c->lexer, MELON_TOKEN_LEFT_BRACKET);
    melM_writeOpVoid(c, MELON_OP_NEWARR);

    if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_RIGHT_BRACKET) == 0)
    {
        return 0;
    }

    while(1)
    {
        melM_writeOp1_25(c, MELON_OP_PUSH, 0);
        melCompileRootExpression(c);
        melM_writeOpVoid(c, MELON_OP_PUSHARR);

        if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_RIGHT_BRACKET) == 0)
        {
            break;
        }

        melPopTypeLexer(&c->lexer, MELON_TOKEN_COMMA);
    }

    return 0;
}

static TRet melCompileSymbolLiteral(Compiler* c)
{
    melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_DOLLAR);

    TBool withLabel = 0;
            
    if (melCheckTokenLexer(&c->lexer, MELON_TOKEN_STRING) == 0)
    {
        withLabel = 1;

        const Token* t = melCurTokenLexer(&c->lexer);
        pushStringConstantToken(c, t);

        melAdvanceLexer(&c->lexer);
    }

    melPopTypeLexer(&c->lexer, MELON_TOKEN_DOLLAR);

    melM_writeOp1_25(c, MELON_OP_NEWSYMBOL, withLabel);

    return 0;
}

static TRet melCompileLiteral(Compiler* c)
{
    melMarkInstLine(c);

    const Token* t = melCurTokenLexer(&c->lexer);
    const struct Operator* op = &operators[t->type];
    
    switch(t->type)
    {
        case MELON_TOKEN_INTEGER:
        {
            char intBuffer[MELON_MAX_INT64_CHAR_COUNT + 1];
            memcpy(&intBuffer, t->start, t->len);
            intBuffer[t->len] = '\0';

            TInteger val = atoi(intBuffer);

            if (
                val <= (MELON_INST_MAX_SIGNED_K + 1)
                && val >= (-MELON_INST_MAX_SIGNED_K)
            )
            {
                melM_writeOp1_s25(c, MELON_OP_PUSHI, val);
            }
            else
            {
                pushIntegerConstant(c, val);
            }

            melAdvanceLexer(&c->lexer);
        }
            break;

        case MELON_TOKEN_FALSE:
            melM_writeOpVoid(c, MELON_OP_PUSHFALSE);
            melAdvanceLexer(&c->lexer);
            break;

        case MELON_TOKEN_NULL:
            melM_writeOpVoid(c, MELON_OP_PUSHNULL);
            melAdvanceLexer(&c->lexer);
            break;

        case MELON_TOKEN_TRUE:
            melM_writeOpVoid(c, MELON_OP_PUSHTRUE);
            melAdvanceLexer(&c->lexer);
            break;

        case MELON_TOKEN_NUMBER:
        {
            char numBuffer[MELON_MAX_NUMBER_CHAR_COUNT];
            memcpy(&numBuffer, t->start, t->len);
            numBuffer[t->len] = '\0';

            pushNumberConstant(c, strtonum(numBuffer));
            melAdvanceLexer(&c->lexer);
        }
            break;

        case MELON_TOKEN_STRING:
            pushStringConstantToken(c, t);
            melAdvanceLexer(&c->lexer);
            break;

        case MELON_TOKEN_LEFT_BRACE:
            melCompileObjectDefinition(c);
            break;

        case MELON_TOKEN_LEFT_BRACKET:
            melCompileArrayDefinition(c);
            break;

        case MELON_TOKEN_FUNC:
        case MELON_TOKEN_PIPE:
        case MELON_TOKEN_FAT_ARROW:
        case MELON_TOKEN_RIGHT_ARROW:
            melM_stackEnsure(c->curFunc->funcStack, c->curFunc->funcStack->top + 1);
            melM_stackSetTop(c->curFunc->funcStack, c->curFunc->funcStack->top + 1);
            melCompileFunction(c, melM_stackTop(c->curFunc->funcStack));
            melM_writeOp1_25(c, MELON_OP_NEWCL, c->curFunc->funcStack->top);
            break;

        case MELON_TOKEN_DOLLAR:
            melCompileSymbolLiteral(c);
            break;

        default:
            return 1;
    }

    return 0;
}

static void melCompileFunctionCall(Compiler* c, TBool methodCall)
{
    TByte argsCount = 0;

    while(1)
    {
        if (melCurTokenLexer(&c->lexer)->type == MELON_TOKEN_EOF)
        {
            melM_error(c, "Expected arguments list but EOF was reached");
            break;
        }

        if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_RIGHT_PAREN) == 0)
        {
            break;
        }

        if (argsCount > 0)
        {
            melPopTypeLexer(&c->lexer, MELON_TOKEN_COMMA);
        }

        melCompileRootExpression(c);

        argsCount++;
    }

    melM_writeOp2_13(c, MELON_OP_CALL, argsCount + (methodCall ? 1 : 0), 1);
}

static void melCompileFactor(Compiler* c)
{
    if (melCompileLiteral(c) != 0)
    {
        melCompileSuffixed(c);
    }
}

static TRet melCompileExpression(Compiler* c, Precedence precedence)
{   
    melMarkInstLine(c);
    
    if (precedence == 0)
    {
        melCompileFactor(c);
        return 0;
    }

    const struct Operator* op = &operators[melCurTokenLexer(&c->lexer)->type];

    if (op->unary == NULL || op->unaryPrecedence != precedence)
    {
        melCompileExpression(c, precedence - 1);
    }
    else
    {
        op->unary(c);
    }

    const Token* curTok = melCurTokenLexer(&c->lexer);

    if (curTok == NULL)
    {
        // No more token. Let some one else decide if it's ok;
        return 0;
    }

    Token infixToken = *curTok;
    const struct Operator* opInfix = &operators[infixToken.type];

    while(opInfix->infix != NULL && opInfix->infixPrecedence == precedence)
    {
        ShortCircuitInfo* sci = NULL;

        if (opInfix->shortCircuit != SCS_NONE)
        {
            sci = melCompileBooleanShortCircuit(c, opInfix->shortCircuit);
        }

        // Consume the infix
        melAdvanceLexer(&c->lexer);
        melCompileExpression(c, precedence - 1);
        opInfix->infix(c, &infixToken);

        if (sci != NULL)
        {
            melFinalizeShortCircuit(c, sci);
            patchJump(c, sci->jmpOff, sci->jmpDest - 1);
        }

        curTok = melCurTokenLexer(&c->lexer);

        if (curTok == NULL)
        {
            break;
        }

        infixToken = *curTok;
        opInfix = &operators[infixToken.type];
    }

    return 0;
}

static void melCompileUnaryOp(Compiler* c)
{
    Token t;
   
    if (melPopLexer(&c->lexer, &t) != 0)
    {
        melM_error(c, "Expected token");
        return;    
    }

    const struct Operator* op = &operators[t.type];
    
    // Compile operand
    melCompileExpression(c, op->unaryPrecedence);
    
    switch(t.type)
    {
        case MELON_TOKEN_MINUS:
            melM_writeOpVoid(c, MELON_OP_NEG);
            break;

        case MELON_TOKEN_EXCLAMATION:
            melM_writeOpVoid(c, MELON_OP_NOT);
            break;

        case MELON_TOKEN_HASH:
            melM_writeOpVoid(c, MELON_OP_SIZEARR);
            break;

        case MELON_TOKEN_AT:
            melM_writeOpVoid(c, MELON_OP_PUSHBLESSING);
            break;

        case MELON_TOKEN_GT:
            melM_writeOp1_25(c, MELON_OP_PUSHITER, 0);
            break;

        case MELON_TOKEN_STAR:
            melM_writeOpVoid(c, MELON_OP_NEXTITER);
            break;

        case MELON_TOKEN_CARET:
            melM_writeOpVoid(c, MELON_OP_BITNOT);
            break;

        default:
            assert(0);
    }
}

static void melCompileBinaryOp(Compiler* c, const Token* t)
{   
    switch(t->type)
    {
        case MELON_TOKEN_MINUS:
            melM_writeOpVoid(c, MELON_OP_SUB);
            break;

        case MELON_TOKEN_STAR:
            melM_writeOpVoid(c, MELON_OP_MUL);
            break;

        case MELON_TOKEN_SLASH:
            melM_writeOpVoid(c, MELON_OP_DIV);
            break;

        case MELON_TOKEN_PLUS:
            melM_writeOpVoid(c, MELON_OP_ADD);
            break;

        case MELON_TOKEN_PIPEPIPE:
            melM_writeOpVoid(c, MELON_OP_OR);
            break;

        case MELON_TOKEN_AMPAMP:
            melM_writeOpVoid(c, MELON_OP_AND);
            break;

        case MELON_TOKEN_LT:
            melM_writeOpVoid(c, MELON_OP_LT);
            break;

        case MELON_TOKEN_GT:
            melM_writeOpVoid(c, MELON_OP_GT);
            break;

        case MELON_TOKEN_LTEQ:
            melM_writeOpVoid(c, MELON_OP_LTE);
            break;

        case MELON_TOKEN_GTEQ:
            melM_writeOpVoid(c, MELON_OP_GTE);
            break;

        case MELON_TOKEN_EQEQ:
            melM_writeOpVoid(c, MELON_OP_EQ);
            break;

        case MELON_TOKEN_NOTEQ:
            // @todo: Dedicated opcode?
            melM_writeOpVoid(c, MELON_OP_EQ);
            melM_writeOpVoid(c, MELON_OP_NOT);
            break;

        case MELON_TOKEN_DOTDOT:
            melM_writeOpVoid(c, MELON_OP_CONCAT);
            break;

        case MELON_TOKEN_AT:
            melM_writeOpVoid(c, MELON_OP_BLESS);
            break;

        case MELON_TOKEN_GTGT:
            melM_writeOpVoid(c, MELON_OP_BITRSHIFT);
            break;

        case MELON_TOKEN_LTLT:
            melM_writeOpVoid(c, MELON_OP_BITLSHIFT);
            break;

        case MELON_TOKEN_AMP:
            melM_writeOpVoid(c, MELON_OP_BITAND);
            break;

        case MELON_TOKEN_PIPE:
            melM_writeOpVoid(c, MELON_OP_BITOR);
            break;

        case MELON_TOKEN_CARET:
            melM_writeOpVoid(c, MELON_OP_POW);
            break;

        case MELON_TOKEN_CARETCARET:
            melM_writeOpVoid(c, MELON_OP_BITXOR);
            break;

        case MELON_TOKEN_PERCENT:
            melM_writeOpVoid(c, MELON_OP_MOD);
            break;

        case MELON_TOKEN_QUESTIONQUESTION:
            melM_writeOpVoid(c, MELON_OP_COALESCE);
            break;

        default:
            assert(0);
    }
}

static TRet patchJump(Compiler* c, TSize jmpLocation, TSize jmpDest)
{
    TSize curOffset = c->curFunc->buffer.size;
    c->curFunc->buffer.size = jmpLocation;
    melM_writeOp1_s25(c, MELON_OP_JMP, (jmpDest - jmpLocation - 1) / sizeof(TVMInst));
    c->curFunc->buffer.size = curOffset;

    return 0;
}

static TRet patchJumpToHere(Compiler* c, TSize fromOffset)
{
    return patchJump(c, fromOffset, c->curFunc->buffer.size);
}

static TRet melCompileIf(Compiler* c)
{
    melPopTypeLexer(&c->lexer, MELON_TOKEN_LEFT_PAREN);
    melCompileRootExpression(c);
    melPopTypeLexer(&c->lexer, MELON_TOKEN_RIGHT_PAREN);
    melM_writeOp1_25(c, MELON_OP_TESTTRUE, 1);
    
    TSize thenOffset = c->curFunc->buffer.size;
    // Reserve some space for the future jump instruction
    c->curFunc->buffer.size += sizeof(TVMInst);

    melCompileStatement(c);

    if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_ELSE) == 0)
    {
        TSize thenEndOffset = c->curFunc->buffer.size;
        c->curFunc->buffer.size += sizeof(TVMInst);

        patchJumpToHere(c, thenOffset);

        melCompileStatement(c);

        patchJumpToHere(c, thenEndOffset);
    }
    else
    {
        patchJumpToHere(c, thenOffset);
    }
    
    return 0;
}

static void melCompileLHSExpr(Compiler* c, const LHSNode* node)
{
    assert(node != NULL);

    switch (node->type)
    {
    case LHS_GLOBAL:
        melM_writeOpVoid(c, MELON_OP_GETGLOBAL);
        break;

    case LHS_OBJ:
        melM_writeOp1_25(c, MELON_OP_GETOBJ, 0);
        break;
    
    case LHS_METHOD_ACCESS:
        melM_writeOp1_25(c, MELON_OP_GETOBJ, 1);
        break;

    case LHS_ARRAY:
        melM_writeOpVoid(c, MELON_OP_GETARR);
        break;

    case LHS_ARRAYPUSH:
        melM_error(c, "Can't use array push operator in expression");
        break;

    case LHS_ARRAYSLICE_COPY:
        melM_writeOp1_25(c, MELON_OP_SLICE, 0);
        break;
    
    case LHS_ARRAYSLICE_START:
        melM_writeOp1_25(c, MELON_OP_SLICE, 1);
        break;

    case LHS_ARRAYSLICE_END:
        melM_writeOp1_25(c, MELON_OP_SLICE, 2);
        break;

    case LHS_ARRAYSLICE_START_END:
        melM_writeOp1_25(c, MELON_OP_SLICE, 3);
        break;

    case LHS_LOCAL:
        melM_writeOp1_25(c, MELON_OP_PUSHLOC, node->varIdx);
        break;
        
    case LHS_UPVAL:
        melM_writeOp1_25(c, MELON_OP_PUSHUPVAL, node->varIdx);
        break;

    case LHS_THIS:
        melM_writeOpVoid(c, MELON_OP_PUSHTHIS);
        break;

    case LHS_CALL:
    case LHS_SUBEXPR:
        break;
    
    default:
        assert(0);
        break;
    }
}

static void melCompileLHSAssignment(Compiler* c, const LHSNode* node)
{
    assert(node != NULL);
    assert(node->type != LHS_NONE);

    switch (node->type)
    {
    case LHS_GLOBAL:
        melM_writeOpVoid(c, MELON_OP_SETGLOBAL);
        break;

    case LHS_OBJ:
        melM_writeOpVoid(c, MELON_OP_SETOBJ);
        break;

    case LHS_ARRAY:
        melM_writeOpVoid(c, MELON_OP_SETARR);
        break;

    case LHS_ARRAYPUSH:
        melM_writeOpVoid(c, MELON_OP_PUSHARR);
        break;

    case LHS_ARRAYSLICE_COPY:
    case LHS_ARRAYSLICE_START:
    case LHS_ARRAYSLICE_END:
    case LHS_ARRAYSLICE_START_END:
        melM_error(c, "Can't use slicing in assignment");
        break;

    case LHS_LOCAL:
        melM_writeOp1_25(c, MELON_OP_STORELOC, node->varIdx);
        break;

    case LHS_UPVAL:
        melM_writeOp1_25(c, MELON_OP_STOREUPVAL, node->varIdx);
        break;

    case LHS_THIS:
        melM_error(c, "Can't assign to 'this'.");
        break;

    case LHS_METHOD_ACCESS:
        melM_error(c, "Can't use method access operator to assign to a property. Use '.' instead.");
        break;

    case LHS_CALL:
        melM_error(c, "Can't assign to function call.");
        break;

    case LHS_SUBEXPR:
        melM_error(c, "Can't assign to sub-expression.");
        break;

    default:
        assert(0);
    }
}

static void melCompilePropertyAccess(Compiler* c, LHSNode* node, TBool methodAccess)
{
    const Token* t = melCurTokenLexer(&c->lexer);

    if (t == NULL)
    {
        melM_error(c, "Unexpected end of input");
        return;
    }

    melCompileLHSExpr(c, node);

    switch(t->type)
    {
        case MELON_TOKEN_NAME:
            node->type = methodAccess ? LHS_METHOD_ACCESS : LHS_OBJ;
            pushStringConstantToken(c, t);
            break;

        default:
            melM_error(c, "Expected valid property name");
            return;
    }

    melAdvanceLexer(&c->lexer);
}

static TRet melCompileLHS(Compiler* c, LHSNode* node)
{
    //@TODO: Maybe DRYer. Looks like the Suffixed expr
    TBool matched = 0;
    node->type = LHS_NONE;

    if (melCheckTokenLexer(&c->lexer, MELON_TOKEN_NAME) == 0)
    {
        const Token* t = melCurTokenLexer(&c->lexer);
        ResolvedVariable var;
        
        melResolveVariable(c, c->curFunc, t->start, t->len, &var);
        
        node->type = (LHSType)var.type;
        node->varIdx = var.idx;

        if (node->type == LHS_GLOBAL)
        {
            pushStringConstantToken(c, t);
        }

        melAdvanceLexer(&c->lexer);
        matched = 1;
    }
    else if (melCheckTokenLexer(&c->lexer, MELON_TOKEN_THIS) == 0)
    {
        if (melM_functionFromObj(c->curFunc->func)->method != 1)
        {
            melM_error(c, "Using 'this' keyword outside a method. Use -> instead of => in the definition.");
            return 1;
        }

        node->type = LHS_THIS;
        matched = 1;

        melAdvanceLexer(&c->lexer);
    }
    else if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_LEFT_PAREN) == 0)
    {
        melCompileExpression(c, MAX_PRECEDENCE);
        melPopTypeLexer(&c->lexer, MELON_TOKEN_RIGHT_PAREN);

        node->type = LHS_SUBEXPR;
        matched = 1;
    }

    if (matched == 1)
    {
        while (1)
        {   
            if (
                melCheckTokenLexer(&c->lexer, MELON_TOKEN_LEFT_PAREN) != 0
                && node->type == LHS_METHOD_ACCESS
            )
            {
                melM_error(c, "Use '->' only to call methods. Use '.' to access a property instead.");
                return 1;
            }

            if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_LEFT_PAREN) == 0)
            {
                melCompileLHSExpr(c, node);
                melCompileFunctionCall(c, node->type == LHS_METHOD_ACCESS);
                node->type = LHS_CALL;
            }
            else if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_DOT) == 0)
            {
                melCompilePropertyAccess(c, node, 0);
            }
            else if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_RIGHT_ARROW) == 0)
            {
                melCompilePropertyAccess(c, node, 1);
            }
            else if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_LEFT_BRACKET) == 0)
            {
                melCompileLHSExpr(c, node);
                
                if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_RIGHT_BRACKET) == 0)
                {
                    // []
                    node->type = LHS_ARRAYPUSH;
                    continue;
                }

                node->type = LHS_ARRAY;

                if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_COLON) == 0)
                {
                    // [:(expr)]
                    node->type = LHS_ARRAYSLICE_END;

                    if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_RIGHT_BRACKET) == 0)
                    {
                        // [:]
                        node->type = LHS_ARRAYSLICE_COPY;
                        continue;
                    }
                }

                // Compile the index or slice start/end expression
                melCompileRootExpression(c);

                if (node->type == LHS_ARRAY && melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_COLON) == 0)
                {
                    // [(expr):]
                    node->type = LHS_ARRAYSLICE_START;
                }

                // Closes normal indexed access or start only slicing
                if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_RIGHT_BRACKET) == 0)
                {
                    continue;
                }

                // [(expr):(expr)]
                node->type = LHS_ARRAYSLICE_START_END;
                melCompileRootExpression(c);
                melPopTypeLexer(&c->lexer, MELON_TOKEN_RIGHT_BRACKET);
            }
            else
            {
                break;
            }
        }
    }

    if (!matched)
    {
        melM_error(c, "Call or assignment expected");
        return 1;
    }

    return 0;
}

static void melCompileSuffixed(Compiler* c)
{
    LHSNode node;

    if (melCompileLHS(c, &node) != 0)
    {
        return;
    }

    melCompileLHSExpr(c, &node);
}

static void patchCall(Compiler* c)
{
    c->curFunc->buffer.size -= sizeof(TVMInst);
    TVMInst callInst = *((TVMInst*)&c->curFunc->buffer.buffer[c->curFunc->buffer.size]);
    assert(melM_getOpCode(callInst) == MELON_OP_CALL);
    melM_writeOp2_13(c, melM_getOpCode(callInst), melM_op_geta13(callInst), 0);
}

static TRet melCompileCallAssign(Compiler* c)
{
    LHSNode node;

    if (melCompileLHS(c, &node) != 0)
    {
        return 1;
    }

    if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_EQ) == 0)
    {
        melCompileRootExpression(c);
        melCompileLHSAssignment(c, &node);
    }
    else if (node.type == LHS_CALL)
    {
        patchCall(c);
    }
    else
    {
        melM_error(c, "Invalid statement. Function call or variable assignment expected.");
        return 1;
    }

    melPopTypeLexer(&c->lexer, MELON_TOKEN_SEMICOLON);

    return 0;
}

static TRet melCompileWhile(Compiler* c)
{
    melPopTypeLexer(&c->lexer, MELON_TOKEN_LEFT_PAREN);
    
    TSize iterateOffset = c->curFunc->buffer.size;

    melCompileRootExpression(c);
    melPopTypeLexer(&c->lexer, MELON_TOKEN_RIGHT_PAREN);
    melM_writeOp1_25(c, MELON_OP_TESTTRUE, 1);

    TSize breakOffset = c->curFunc->buffer.size;
    // Reserve some space for the future jump instruction
    c->curFunc->buffer.size += sizeof(TVMInst);

    LoopInfo* li = melPushLoop(c, iterateOffset);

    melCompileStatement(c);

    melM_writeOp1_s25(c, MELON_OP_JMP, (iterateOffset - c->curFunc->buffer.size - 1) / sizeof(TVMInst));
    li->loopEnd = c->curFunc->buffer.size;

    patchJumpToHere(c, breakOffset);

    melCloseInnerLoop(c);

    return 0;
}

static TRet melCompileFor(Compiler* c)
{
    melPopTypeLexer(&c->lexer, MELON_TOKEN_LEFT_PAREN);
    melPopTypeLexer(&c->lexer, MELON_TOKEN_LET);
    
    TSize valIdx = 0;
    TSize oldBlockTop;

    melNewBlock(c, &oldBlockTop);

    if (melCompileLocalDeclaration(c, &valIdx) != 0)
    {   
        melEndBlock(c, oldBlockTop);
        return 1;
    }

    melPopTypeLexer(&c->lexer, MELON_TOKEN_IN);

    // Create a hidden local slot for the iterator.
    // That's why we are not using melAddLocal.
    TSize itIdx = c->curFunc->usedVariables++;
    memset(&c->curFunc->variables[itIdx], 0, sizeof(VariableInfo));
    c->curFunc->variables[itIdx].level = c->curFunc->variables[valIdx].level;

    melCompileRootExpression(c);
    melPopTypeLexer(&c->lexer, MELON_TOKEN_RIGHT_PAREN);
    melM_writeOp1_25(c, MELON_OP_PUSHITER, itIdx + 1);

    TSize iterateOffset = c->curFunc->buffer.size;
    melM_writeOp2_13(c, MELON_OP_SETUPFOR, valIdx, itIdx);
    TSize continueOffset = c->curFunc->buffer.size;
    // Reserve some space for the future jump instruction
    c->curFunc->buffer.size += sizeof(TVMInst);

    LoopInfo* li = melPushLoop(c, iterateOffset);

    melCompileStatement(c);

    melM_writeOp1_s25(c, MELON_OP_JMP, (iterateOffset - c->curFunc->buffer.size - 1) / sizeof(TVMInst));
    li->loopEnd = c->curFunc->buffer.size;
    patchJumpToHere(c, continueOffset);

    melCloseInnerLoop(c);

    melEndBlock(c, oldBlockTop);

    return 0;
}

static TRet melRemoveLocalAt(Compiler* c, FunctionDef* fd, TSize slot)
{
    melM_log("Removing local \"%.*s\" from slot %ld\n", fd->variables[slot].len, fd->variables[slot].name, slot);
    fd->variables[slot].isDeleted = 1;
    return 0;
}

static TRet melRemoveLocal(Compiler* c, FunctionDef* fd, const char* name, TSize len)
{
    TSize slot = 0;
    
    if (melFindLocal(c, fd, name, len, &slot) != 0)
    {
        return 1;
    }

    return melRemoveLocalAt(c, fd, slot);
}

static TRet melFindFreeLocal(Compiler* c, FunctionDef* fd, TSize* out)
{
    for (TSize i = 0; i < fd->usedVariables; i++)
    {
        const VariableInfo* cur = &fd->variables[i];

        if (cur->isDeleted == 1)
        {
            *out = i;
            return 0;
        }
    }

    assert(fd->usedVariables + 1 < MELON_COMP_MAX_LOCAL_VARIABLES);
    *out = fd->usedVariables++;
    return 0;
}

static TRet melAddLocal(Compiler* c, FunctionDef* fd, const char* name, TSize len, TSize* outSlot)
{    
    TSize slot = 0;
    
    if (melFindLocal(c, fd, name, len, &slot) == 0)
    {
        if (fd->variables[slot].level == c->curFunc->blockLevel)
        {
            melM_error(c, "Variable is already defined");
            return 1;
        }
    }

    if (melFindFreeLocal(c, fd, &slot) != 0)
    {
        return 1;
    }

    VariableInfo* vi = &fd->variables[slot];

    melM_log("Adding local \"%.*s\" to slot %ld\n", len, name, slot);

    vi->isUpvalue = 0;
    vi->len = len;
    vi->name = name;
    vi->level = c->curFunc->blockLevel;
    vi->isDeleted = 0;

    assert(fd->blockVariablesTop + 1 < MELON_COMP_MAX_LOCAL_VARIABLES);
    fd->blockVariables[fd->blockVariablesTop++] = slot;

    if (outSlot != NULL)
    {
        *outSlot = slot;
    }

    return 0;
}

static TRet melCompileLocalDeclaration(Compiler* c, TSize* varIdx)
{
    const Token* t = melCurTokenLexer(&c->lexer);

    if (t->type != MELON_TOKEN_NAME)
    {
        melM_error(c, "Expected variable name");
        return 1;
    }

    melAddLocal(c, c->curFunc, t->start, t->len, varIdx);

    melAdvanceLexer(&c->lexer);

    return 0;
}

static TRet melCompileLocal(Compiler* c)
{
    TSize varIdx = 0;

    if (melCompileLocalDeclaration(c, &varIdx) != 0)
    {
        return 1;
    }

    if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_EQ) == 0)
    {
        melCompileRootExpression(c);
        melM_writeOp1_25(c, MELON_OP_STORELOC, varIdx);
    }

    melPopTypeLexer(&c->lexer, MELON_TOKEN_SEMICOLON);

    return 0;
}

static TRet melCompileReturn(Compiler* c)
{
    TVMInstK retNum = 0;

    if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_SEMICOLON) != 0)
    {
        melCompileRootExpression(c);
        melPopTypeLexer(&c->lexer, MELON_TOKEN_SEMICOLON);
        retNum = 1;
    }

    melM_writeOp1_25(c, MELON_OP_RET, retNum);

    return 0;
}

static TRet melAddNamedFunctionLocal(
    Compiler* c, 
    FunctionDef* namedFunc, 
    TSize funcId, 
    FunctionDef* dstFunc
)
{
    Function* fn = melM_functionFromObj(namedFunc->func);
    String* nameStr = melM_strFromObj(fn->name);
    const char* nameCStr = melM_strDataFromObj(fn->name);

    TSize localVar;
    melAddLocal(c, dstFunc, nameCStr, nameStr->len, &localVar);

    if (funcId == 0)
    {
        melM_writeOp1_25(c, MELON_OP_STORETOPCL, localVar);
    }
    else
    {
        melM_writeOp1_25(c, MELON_OP_NEWCL, funcId);
        melM_writeOp1_25(c, MELON_OP_STORELOC, localVar);
    }

    return 0;
}

static TRet melCompileNamedFunction(Compiler* c)
{
    melM_stackEnsure(c->curFunc->funcStack, c->curFunc->funcStack->top + 1);
    melM_stackSetTop(c->curFunc->funcStack, c->curFunc->funcStack->top + 1);
    FunctionDef* fd = melM_stackTop(c->curFunc->funcStack);

    melCompileFunction(c, fd);
    Function* fn = melM_functionFromObj(fd->func);

    if (fn->name == NULL)
    {
        melM_error(c, "Only named functions can appear outside expressions.");
        return 1;
    }

    melAddNamedFunctionLocal(c, fd, c->curFunc->funcStack->top, c->curFunc);

    return 0;
}

static TRet melCompileBreak(Compiler* c, TBool cont)
{
    LoopBreak* lb = melAddBreak(c);

    if (lb == NULL)
    {
        melM_error(c, "Can't use 'break' or 'continue' outside of a loop");
        return 1;
    }

    lb->cont = cont;

    // Reserve some space for the future JMP
    c->curFunc->buffer.size += sizeof(TVMInst);
    melPopTypeLexer(&c->lexer, MELON_TOKEN_SEMICOLON);

    return 0;
}

static TRet melNewBlock(Compiler* c, TSize* blockTop)
{
    assert(blockTop != NULL);

    c->curFunc->blockLevel++;
    *blockTop = c->curFunc->blockVariablesTop;
    melM_log("Starting level %ld\n", c->curFunc->blockLevel);

    return 0;
}

static TRet melEndBlock(Compiler* c, TSize oldBlockTop)
{
    melM_log("Closing level %ld\n", c->curFunc->blockLevel);
    assert(c->curFunc->blockLevel > 0);
    c->curFunc->blockLevel--;

    for (TSize i = c->curFunc->blockVariablesTop; i > oldBlockTop; i--)
    {
        melRemoveLocalAt(c, c->curFunc, c->curFunc->blockVariables[i - 1]);
    }

    c->curFunc->blockVariablesTop = oldBlockTop;

    return 0;
}

static TRet melCompileStatement(Compiler* c)
{
    melMarkInstLine(c);
    
    //@TODO: This if/else chain will probably kill the pipeline
    if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_LET) == 0)
    {
        melCompileLocal(c);
    }
    else if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_IF) == 0)
    {
        melCompileIf(c);
    }
    else if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_LEFT_BRACE) == 0)
    {
        TSize prevBlockVarsTop;
        melNewBlock(c, &prevBlockVarsTop);

        while (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_RIGHT_BRACE) != 0)
        {
            melCompileStatement(c);
        }

        melEndBlock(c, prevBlockVarsTop);    
    }
    else if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_WHILE) == 0)
    {
        melCompileWhile(c);
    }
    else if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_FOR) == 0)
    {
        melCompileFor(c);
    }
    else if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_BREAK) == 0)
    {
        melCompileBreak(c, 0);
    }
    else if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_CONTINUE) == 0)
    {
        melCompileBreak(c, 1);
    }
    else if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_RETURN) == 0)
    {
        melCompileReturn(c);
    }
    else if (melCheckTokenLexer(&c->lexer, MELON_TOKEN_FUNC) == 0)
    {
        melCompileNamedFunction(c);
    }
    else
    {
        melCompileCallAssign(c);
    }

    return 0;
}

static TRet melUpdateSourceLine(Compiler* c)
{
    DebugSymbols* ds = &(melM_functionFromObj(c->curFunc->func)->debug);

    if (melGrowBuffer((void**)&ds->lines, &ds->reserved, sizeof(TSize), c->curFunc->pc + 1) != 0)
    {
        return 1;
    }

    ds->lines[c->curFunc->pc] = c->curInstLine;
    ds->count++;
    c->curFunc->pc++;

    return 0;
}

static TRet melMarkInstLine(Compiler* c)
{
    c->curInstLine = c->lexer.line;
    return 0;
}

static TRet melCompileFunction(Compiler* c, FunctionDef* funcDef)
{
    if (melCreateFunctionDef(c, funcDef) != 0)
    {
        melM_error(c, "Can't create function definition");
        return 1;
    }

    funcDef->parent = c->curFunc;
    c->curFunc = funcDef;

    TBool isMainFunc = funcDef == &c->main;

    Function* defFn = melM_functionFromObj(funcDef->func);
    defFn->debug.file = c->lexer.file;

    // Main function is always a method because the 'this' variable
    // points to the module object (even if this was never referenced)
    if (isMainFunc)
    {
        defFn->method = 1;
        defFn->args = 1;
    }

    if (!isMainFunc)
    {
        if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_FUNC) == 0)
        {
            if (melCheckTokenLexer(&c->lexer, MELON_TOKEN_NAME) == 0)
            {
                const Token* t = melCurTokenLexer(&c->lexer);
                defFn->name = melNewString(c->vm, t->start, t->len);
                melAdvanceLexer(&c->lexer);
            }
        }

        TSize args = 0;

        if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_PIPE) == 0)
        {
            while(melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_PIPE) != 0)
            {
                const Token* t = melCurTokenLexer(&c->lexer);

                if (t->type == MELON_TOKEN_ELLIPSIS)
                {
                    defFn->rest = 1;
                    melAdvanceLexer(&c->lexer);
                    t = melCurTokenLexer(&c->lexer);
                }

                if (t->type != MELON_TOKEN_NAME)
                {
                    melM_error(c, "Expected argument name");
                    return 1;
                }

                melAddLocal(c, c->curFunc, t->start, t->len, NULL);

                args++;

                melAdvanceLexer(&c->lexer);

                if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_PIPE) == 0)
                {
                    break;
                }

                if (defFn->rest)
                {
                    melM_error(
                        c, 
                        "The ... (rest) argument should be the last one in a function definition"
                    );
                }

                melPopTypeLexer(&c->lexer, MELON_TOKEN_COMMA);
            }
        }

        if (melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_RIGHT_ARROW) == 0)
        {
            defFn->method = 1;
        }
        else
        {
            melPopTypeLexer(&c->lexer, MELON_TOKEN_FAT_ARROW);
        }

        defFn->args = args;
    }
    else
    {
        defFn->name = melNewString(c->vm, "main", strlen("main"));
    }

    if (!isMainFunc)
    {
        if (defFn->name != NULL)
        {
            // Makes the closure reachable from itself
            melAddNamedFunctionLocal(c, funcDef, 0, funcDef);
        }
    
        melPopTypeLexer(&c->lexer, MELON_TOKEN_LEFT_BRACE);
    }

    while (1)
    {
        if (melCurTokenLexer(&c->lexer)->type == MELON_TOKEN_EOF)
        {
            if (isMainFunc)
            {
                break;
            }

            melM_error(c, "Expected right brace at the end of function definition");
            return 1;
        }

        if (!isMainFunc && melAdvanceIfTypeLexer(&c->lexer, MELON_TOKEN_RIGHT_BRACE) == 0)
        {
            break;
        }
        
        if (melCompileStatement(c) != 0)
        {
            return 1;
        }
    }

    /*
        Deciding if the function's body returned something or not is
        a hard problem. We can't simply look at the last generated instruction
        because we have no way to tell if it is part of a branch. This is also true
        for return statements without adding complexity.
        To make sure that a function always returns we are thus always appending
        an extra return instruction. In the worst case scenarios this means 4 bytes more
        for every function in the bytecode.
    */

    melM_writeOp1_25(c, MELON_OP_RET, 0);

    /* @TODO:
        I dont like the fact that we are denormalizing function-related data. 
        Maybe what's in FunctionDef should be moved inside Function so that
        we can fill the same data structure we are going to serialize without the
        memory overhead? Yet forcing the VM to carry arround useless information
        seems way worse, but maybe it's not useless information because it would
        be required by a debugger anyway? I need to think more about the issue.

        NOTE: This is true in general for most of the structures below (ie: UpvaluesInfo)
    */

    defFn->localsSlots = funcDef->usedVariables;
    defFn->code = (Instruction*)funcDef->buffer.buffer;
    defFn->size = funcDef->buffer.size;

    defFn->funcPrototype.count = funcDef->funcStack->top;
    defFn->funcPrototype.prototypes = malloc(sizeof(GCItem*) * defFn->funcPrototype.count);
    assert(defFn->funcPrototype.prototypes != NULL);

    TSize i = funcDef->funcStack->top - 1;

    while (!melM_stackIsEmpty(funcDef->funcStack))
    {
        defFn->funcPrototype.prototypes[i] = melM_stackPop(funcDef->funcStack)->func;
        GCItem* fObj = defFn->funcPrototype.prototypes[i]; 
        Function* f = melM_functionFromObj(fObj);
        f->args += (f->method ? 1 : 0);

        --i;
    }

    defFn->upvaluesInfos.count = funcDef->usedUpvalues;
    defFn->upvaluesInfos.infos = malloc(sizeof(UpvalueInfo) * defFn->upvaluesInfos.count);
    
    for (i = 0; i < funcDef->usedUpvalues; i++)
    {
        UpvalueInfo* ui = &defFn->upvaluesInfos.infos[i];
        ui->idx = funcDef->upvalues[i].index;
        ui->instack = funcDef->upvalues[i].isLocal;
    }

    c->curFunc = funcDef->parent;

    return 0;
}

static TRet melWriteBufferCompiler(void* ctx, const void* data, TSize num)
{
    Compiler* c = (Compiler*)ctx;
    return melWriteBuffer(&(c->curFunc->buffer), data, num);
}

static void melWriteBufferErrorCompiler(void* ctx, const char* message)
{
    melM_error((Compiler*)ctx, message);
}

static TRet melCreateFunctionDef(Compiler* c, FunctionDef* funcDef)
{
    memset(funcDef, 0, sizeof(FunctionDef));

    // Sadly we have to create this on the heap for now because FunctionStack
    // is a recursive structure
    funcDef->funcStack = malloc(sizeof(FunctionStack));

    if (melCreateStack(funcDef->funcStack, MELON_COMP_MIN_FUNC_STACK_SIZE, sizeof(FunctionDef)) != 0)
    {
        melM_error(c, "Can't create functions stack");
        return 1;
    }

    if (melCreateBuffer(&funcDef->buffer, 0) != 0)
    {
        melM_error(c, "Can't create function code buffer");
        return 1;
    }

    funcDef->func = melNewFunction(c->vm);

    Function* fn = melM_functionFromObj(funcDef->func);
    fn->debug.source = c->lexer.source;
    fn->debug.sourceSize = c->lexer.sourceSize;

    return 0;
}

static void printError(const char* type, void* ctx, const char* file, const char* message, TSize line, TSize col)
{
    Compiler* c = (Compiler*)ctx;
    c->hasErrors = 1;

    melPrintErrorAtSourceUtils(
        c->vm, 
        type,
        message,
        file,
        c->lexer.source, 
        c->lexer.sourceSize, 
        line, 
        col, 
        MELON_DEFAULT_ERROR_LEN, 
        MELON_DEFAULT_ERROR_CTX
    );
}

static void lexerError(void* ctx, const char* file, const char* message, TSize line, TSize col)
{
    printError("Parser", ctx, file, message, line, col);
    assert(0);
}

static void compilerError(void* ctx, const char* file, const char* message, TSize line, TSize col)
{
    printError("Compiler", ctx, file, message, line, col);
    assert(0);
}

TRet melCreateCompiler(Compiler* c, VM* vm, const char* source, TSize len)
{
    return melCreateCompilerFile(c, vm, NULL, source, len);
}

TRet melCreateCompilerFile(Compiler* c, VM* vm, const char* file, const char* source, TSize len)
{
    c->hasErrors = 0;
    c->error = &compilerError;
    c->errorCtx = c;
    c->curInstLine = 0;
    c->curFunc = NULL;
    c->module = 0;
    c->vm = vm;
    c->lastShortCircuit = 0;

    if (melCreateStack(&c->loopStack, MELON_COMP_MIN_LOOP_STACK_SIZE, sizeof(LoopInfo)) != 0)
    {
        melM_error(c, "Can't allocate loop stack");
        return 1;
    }

    if (melCreateStack(&c->shortCircuitStack, 8, sizeof(ShortCircuitInfo)) != 0)
    {
        melM_error(c, "Can't allocate short circuit stack");
        return 1;
    }

    if (melCreateLexerFile(&c->lexer, file, source, len) != 0)
    {
        melM_error(c, "Can't create lexer");
        return 1;
    }

    c->lexer.error = &lexerError;
    c->lexer.errorCtx = c;

    if (melAdvanceLexer(&c->lexer) != 0)
    {
        return 1;
    }

    c->serializer.ctx = (void*)c;
    c->serializer.writer = &melWriteBufferCompiler;
    c->serializer.error = &melWriteBufferErrorCompiler;

    return 0;
}

TRet melRunCompiler(Compiler* c)
{
    melSetPauseGC(c->vm, &c->vm->gc, 1);
    melAdvanceLexer(&c->lexer);

    if (melCompileFunction(c, &c->main) != 0)
    {
        melSetPauseGC(c->vm, &c->vm->gc, 0);
        return 1;
    }

    melSetPauseGC(c->vm, &c->vm->gc, 0);
    return 0;
}