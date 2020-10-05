#ifndef __melon__vm_h__
#define __melon__vm_h__

#include "melon/core/macros.h"
#include "melon/core/types.h"
#include "melon/core/stack.h"
#include "melon/core/value.h"
#include "melon/core/object.h"
#include "melon/core/gc.h"
#include "melon/core/function.h"
#include "melon/core/strings_table.h"

#define MELON_VM_SIPHASH_KEY_SIZE 16
#define MELON_VM_MIN_CALLSTACK_SIZE 16

#define melM_fatal(vm, format, ...) { melErrorVM((vm), format melM_vaargs(__VA_ARGS__)); exit(1); } 0

#define MELON_VM_MAGIC_BYTES_COUNT 5
extern const TByte MAGIC_BYTES[];

extern const TInteger INT_CHECK_VALUE;
extern const TNumber NUM_CHECK_VALUE;

extern const char* MELON_TYPES_NAMES[];

#pragma pack(push, 1)

struct VM;
typedef struct Deserializer Deserializer;

typedef struct CallFrame {
    StackSize stackStart;
    StackSize stackBase;
    GCItem* closure;
    TSize pc;
    Function* function;
    TUint16 expRet;
} CallFrame;

melM_defineStackStruct(CallStack, CallFrame)

typedef struct Upvalue {
    Value* value;
    Value closed;
    struct Upvalue* next;
    struct Upvalue* prev;
    TSize refCount;
} Upvalue;
#pragma pack(pop)

struct StrFormat;
typedef void(*PrintFn)(struct StrFormat*, void*);

typedef struct VMConfig {
    PrintFn printFn;
    void* printCtx;
} VMConfig;

typedef struct VM {
    Stack stack;
    CallStack callStack;
    Upvalue* openUpvalues;
    GCItem* global;
    GC gc;
    VMConfig config;
    TSymbolID nextSymbolId;
    StringsTable stringsTable;
    char hashKey[MELON_VM_SIPHASH_KEY_SIZE];
} VM;

TRet melCreateVM(VM* vm, VMConfig* config);

TRet melLoadProgramVM(
    VM* vm, 
    TByte* program, 
    TSize size, 
    GCItem** prog, 
    const char* path, 
    Deserializer* ds
);

TRet melRunProgramVM(VM* vm, GCItem* pi);
TRet melRunMainFunctionVM(VM* vm, GCItem* f);
TRet melRunModuleFunctionVM(VM* vm, GCItem* f);
TRet melPushModuleObjectVM(VM* vm, GCItem* fnObj);

TRet melCallFunctionVM(VM* vm, GCItem* cl, GCItem* f, TUint16 expRet);
TRet melCallClosureVM(VM* vm, TSize callArgs, TBool tail, TUint16 expRet);
TRet melCallClosureSyncVM(VM* vm, TSize callArgs, TBool tail, TUint16 expRet);

TRet melPerformCustomOpVM(VM* vm, MelonObjectSymbols op, TUint8 args);

CallFrame* melGetTopCallFrameVM(VM* vm);
TRet melReleaseUpvalueVM(VM* vm, Upvalue* upvalue);
Value* melGetLocalVM(VM* vm, CallFrame* cf, TSize slot, TBool ignoreMethod);
TRet melSetLocalVM(VM* vm, CallFrame* cf, TSize slot, Value* val, TBool ignoreMethod);
void melErrorVM(VM* vm, const char *format, ...);
void melPrintVM(VM* vm, struct StrFormat* sf);

TRet melCreateGlobalSymbolKey(VM* vm, const char* k, Value* val);

#endif // __melon__vm_h__