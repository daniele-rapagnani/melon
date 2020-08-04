#ifndef __melon__function__
#define __melon__function__

#include "melon/core/types.h"
#include "melon/core/value.h"
#include "melon/core/gc_item.h"

#define melM_functionFromObj(obj) ((Function*)((TPtr)(obj) + sizeof(GCItem)))

typedef struct VM VM;

typedef struct DebugSymbols {
    TSize count;
    TSize reserved;
    TSize* lines;
    const char* file;
    const char* source;
    TSize sourceSize;
} DebugSymbols;

typedef union Instruction {
    TUint32 inst;
} Instruction;

typedef TByte (*NativeFunctionPtr)(struct VM*);

typedef struct StaticConstants {
    TSize count;
    TSize reserved;
    Value* constants;
} StaticConstants;

struct Function;
typedef struct Function Function;

typedef struct FunctionPrototypes {
    TSize count;
    TSize reserved;
    GCItem** prototypes;
} FunctionPrototypes;

typedef struct UpvalueInfo {
    TByte instack;
    TByte idx;
} UpvalueInfo;

typedef struct UpvaluesInfos {
    TSize count;
    TSize reserved;
    UpvalueInfo* infos;
} UpvaluesInfos;

typedef struct Function {
    TInteger localsSlots;
    TByte args;
    TBool rest;
    Instruction* code;
    StaticConstants constants;
    NativeFunctionPtr native;
    GCItem* name;
    FunctionPrototypes funcPrototype;
    UpvaluesInfos upvaluesInfos;
    TSize size;
    TBool method;
    DebugSymbols debug;
} Function;

GCItem* melNewFunction(VM* vm);
TRet melFreeFunction(VM* vm, GCItem* func);

#endif // __melon__function__