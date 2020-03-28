#include "melon/modules/debug/debug_module.h"

#include "melon/modules/modules.h"
#include "melon/core/tstring.h"
#include "melon/core/array.h"
#include "melon/core/utils.h"

#include <stdlib.h>

static TByte printStackFunc(VM* vm)
{
    melPrintStackUtils(vm);
    return 0;
}

static TByte printCallstackFunc(VM* vm)
{
    melPrintCallStackUtils(vm);
    return 0;
}

static TByte errorFunc(VM* vm)
{
    melM_arg(vm, error, MELON_TYPE_STRING, 0);
    String* str = melM_strFromObj(error->pack.obj);
    const char* cstr = melM_strDataFromObj(error->pack.obj);
    melM_fatal(vm, "%.*s\n", str->len, cstr);
    return 0;
}

static TByte getCallstackFunc(VM* vm)
{
    GCItem* arr = melNewArray(vm);
    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* arrVal = melM_stackAllocRaw(&vm->stack);
    arrVal->type = arr->type;
    arrVal->pack.obj = arr;

    for (TSize i = 0; i < vm->callStack.top - 1; i++)
    {
        CallFrame* cf = melM_stackGet(&vm->callStack, i);
        
        Value funcNameVal;
        funcNameVal.type = MELON_TYPE_CLOSURE;
        funcNameVal.pack.obj = cf->closure;
        
        melPushArray(vm, arr, &funcNameVal);
    }

    return 1;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "printStack", 0, 0, &printStackFunc},
    { "printCallstack", 0, 0, &printCallstackFunc},
    { "error", 1, 0, &errorFunc},
    { "getCallstack", 0, 0, &getCallstackFunc},
    NULL
};

TRet melDebugModuleInit(VM* vm)
{
    return melNewModule(vm, funcs);
}