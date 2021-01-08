#include "melon/modules/integer/integer_module.h"
#include "melon/modules/modules.h"
#include "melon/core/tstring.h"

/***
 * @module
 * 
 * This module provides basic utility functions to interact with [`Integer`](integer.md) values.
 */

#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

/***
 * Converts a `String` to an `Integer`.
 * 
 * @arg strInt A string representing an integer number
 * 
 * @returns A valid `Integer` or `null`.
 */

static TByte fromStringFunc(VM* vm)
{
    melM_arg(vm, strInt, MELON_TYPE_STRING, 0);

    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* arrVal = melM_stackAllocRaw(&vm->stack);
    arrVal->type = MELON_TYPE_INTEGER;

    String* strObj = melM_strFromObj(strInt->pack.obj);
    const char* str = melM_strDataFromObj(strInt->pack.obj);

    for (TSize i = 0; i < strObj->len; i++)
    {
        if (i == 0 && (str[i] == '-' || str[i] == '+'))
        {
            continue;
        }

        if (!isdigit(str[i]))
        {
            arrVal->type = MELON_TYPE_NULL;
            return 1;
        }
    }

    arrVal->pack.value.integer = atoi(str);

    return 1;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "fromString", 1, 0, &fromStringFunc },
    { NULL, 0, 0, NULL }
};

TRet melIntegerModuleInit(VM* vm)
{
    return melNewModule(vm, funcs);
}