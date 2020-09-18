#ifndef __melon__compiler_h__
#define __melon__compiler_h__

#include "melon/core/config.h"
#include "melon/core/lexer.h"
#include "melon/core/types.h"
#include "melon/core/buffer.h"
#include "melon/core/serializer.h"
#include "melon/core/stack.h"
#include "melon/core/gc_item.h"
#include "melon/core/vm.h"

typedef struct FunctionStack FunctionStack;

typedef struct VariableInfo
{
    const char* name;
    TSize len;
    TUint16 level;
    TBool isUpvalue;
    TBool isDeleted;
} VariableInfo;

typedef struct CompiledUpvalue
{
    const char* name;
    TSize len;
    TBool isLocal;
    TSize index;
} CompiledUpvalue;

typedef struct FunctionDef {
    Buffer buffer;
    GCItem* func;
    struct FunctionDef* parent;
    FunctionStack* funcStack;
    VariableInfo variables[MELON_COMP_MAX_LOCAL_VARIABLES];
    TUint16 blockVariables[MELON_COMP_MAX_LOCAL_VARIABLES];
    TUint16 blockVariablesTop;
    TUint16 blockLevel;
    CompiledUpvalue upvalues[MELON_COMP_MAX_UPVALUES];
    TSize pc;
    TSize usedVariables;
    TSize usedUpvalues;
} FunctionDef;

melM_defineStackStruct(FunctionStack, FunctionDef)

typedef struct LoopBreak {
    TBool cont;
    TSize patchOffset;
    struct LoopBreak* next;
} LoopBreak;

typedef struct LoopInfo {
    TSize loopStart;
    TSize loopEnd;
    LoopBreak* breaks;
} LoopInfo;

melM_defineStackStruct(LoopStack, LoopInfo)

typedef struct ShortCircuitInfo {
    TSize checkStartOff;
    TSize jmpOff;
    TSize jmpDest;
    TBool testTrue;
} ShortCircuitInfo;

melM_defineStackStruct(ShortCircuitStack, ShortCircuitInfo)

typedef void (*CompilerErrorCb)(
    void* ctx, 
    const char* message,
    const char* file,
    TSize line, 
    TSize column
);

typedef struct Compiler {
    Lexer lexer;
    Serializer serializer;
    FunctionDef main;
    FunctionDef* curFunc;
    VM* vm;
    TBool hasErrors;
    TBool module;
    TSize curInstLine;
    LoopStack loopStack;
    ShortCircuitStack shortCircuitStack;
    TBool lastShortCircuit;

    CompilerErrorCb error;
    void* errorCtx;
} Compiler;

TRet melCreateCompilerFile(Compiler* c, VM* vm, const char* file, const char* source, TSize len);
TRet melCreateCompiler(Compiler* c, VM* vm, const char* source, TSize len);
TRet melRunCompiler(Compiler* c);

#endif // __melon__compiler_h__