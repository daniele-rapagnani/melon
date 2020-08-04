#include "melon/modules/function/function_module.h"
#include "melon/modules/modules.h"
#include "melon/core/closure.h"
#include "melon/core/array.h"
#include "melon/core/tstring.h"

#include <stdlib.h>
#include <assert.h>

static TByte callFunc(VM* vm)
{
    melM_arg(vm, func, MELON_TYPE_CLOSURE, 0);
    melM_arg(vm, args, MELON_TYPE_ARRAY, 1);

    melM_stackPush(&vm->stack, func);

    Array* arrObj = melM_arrayFromObj(args->pack.obj);
    for (TSize i = 0; i < arrObj->count; i++)
    {
        Value* val = melGetIndexArray(vm, args->pack.obj, i);
        melM_stackPush(&vm->stack, val);
    }

    melCallClosureSyncVM(vm, arrObj->count, 0, 1);

    // @TODO: Multiple returns not supported
    return 1;
}

static TByte getNameFunc(VM* vm)
{
    melM_arg(vm, func, MELON_TYPE_CLOSURE, 0);
    Closure* cl = melM_closureFromObj(func->pack.obj);
    Function* fn = melM_functionFromObj(cl->fn);

    Value res;
    res.type = MELON_TYPE_STRING;

    if (fn->name != NULL)
    {
        res.pack.obj = fn->name;
    }
    else
    {
        res.pack.obj = melNewString(vm, "@anonymous@", strlen("@anonymous@"));
    }

    melM_stackPush(&vm->stack, &res);
    return 1;
}

static TByte getArgsCountFunc(VM* vm)
{
    melM_arg(vm, func, MELON_TYPE_CLOSURE, 0);
    Closure* cl = melM_closureFromObj(func->pack.obj);
    Function* fn = melM_functionFromObj(cl->fn);

    Value res;
    res.type = MELON_TYPE_INTEGER;
    res.pack.value.integer = fn->args;
    melM_stackPush(&vm->stack, &res);
    return 1;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "call", 2, 0, &callFunc },
    { "getName", 1, 0, &getNameFunc },
    { "getArgsCount", 1, 0, &getArgsCountFunc },
    { NULL, 0, 0, NULL }
};

TRet melFunctionModuleInit(VM* vm)
{
    return melNewModule(vm, funcs);
}