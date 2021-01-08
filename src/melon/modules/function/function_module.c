#include "melon/modules/function/function_module.h"
#include "melon/modules/modules.h"
#include "melon/core/closure.h"
#include "melon/core/array.h"
#include "melon/core/tstring.h"

/***
 * @module
 * 
 * This module can be used to programmatically interact with [`Function`](function.md)
 * values. Most of the functions in this module can be built upon to enable
 * basic functional programming patterns.
 */

#include <stdlib.h>
#include <assert.h>

/***
 * Calls a function value programmatically
 * 
 * @arg func The function to be invoked
 * @arg args An array of arguments with which the function will be invoked
 * 
 * @returns Whatever the original function returned
 */

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

/***
 * Gets the name of a function value
 * 
 * @arg func The function to extract the name from
 * 
 * @returns The function's name if any, `null` otherwise
 */

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

/***
 * Gets the number of arguments a given function expects
 * 
 * @arg func The function to inspect
 * 
 * @returns An integer representing the number of expected arguments
 */

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

/***
 * Returns the path to the file in which a given function value was defined.
 * If the string is built-in or was defined in some non-standard way `null`
 * will be returned.
 * 
 * @arg func The function to inspect
 * 
 * @returns A string containing the path to the file or `null`
 */

static TByte getFileFunc(VM* vm)
{
    melM_arg(vm, func, MELON_TYPE_CLOSURE, 0);
    Closure* cl = melM_closureFromObj(func->pack.obj);
    Function* fn = melM_functionFromObj(cl->fn);

    Value res;
    res.type = MELON_TYPE_NULL;

    if (fn->debug.file != NULL)
    {
        res.type = MELON_TYPE_STRING;
        res.pack.obj = melNewString(vm, fn->debug.file, strlen(fn->debug.file));
    }

    melM_stackPush(&vm->stack, &res);

    return 1;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "call", 2, 0, &callFunc },
    { "getName", 1, 0, &getNameFunc },
    { "getArgsCount", 1, 0, &getArgsCountFunc },
    { "getFile", 1, 0, &getFileFunc },
    { NULL, 0, 0, NULL }
};

TRet melFunctionModuleInit(VM* vm)
{
    return melNewModule(vm, funcs);
}