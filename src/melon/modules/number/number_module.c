#include "melon/modules/number/number_module.h"
#include "melon/modules/modules.h"
#include "melon/core/tstring.h"

/***
 * @module
 * 
 * This module provides basic utility functions to interact with [`Number`](number.md) values.
 */

#include <stdlib.h>
#include <assert.h>

//@TODO: This was taken from the Lexer and slightly modified.
//       should probably be in some kind of utils.
static TBool isNumber(const char* str, TSize len)
{
    TBool dot = 0;
    TBool exp = 0;
    TBool prevExp = 0;
    TSize s = 0;

    for (TSize i = 0; i < len; i++)
    {
        char c = str[i];
        char nc = i < len - 1 ? str[i + 1] : '\0';
        char nnc = i < len - 2 ? str[i + 2] : '\0';

        if (c != '-' && c != '+' && c != '.' && c != 'E' && c != 'e' && (c < '0' || c > '9'))
        {
            return 0;
        }

        if (c == '-' || c == '+')
        {
            if (!prevExp && s > 1)
            {
                return 0;
            }

            if (nc < '0' || nc > '9')
            {
                return 0;
            }
        }
        else if (c == '.')
        {
            if (dot || exp)
            {
                return 0;
            }

            if (nc < '0' || nc > '9')
            {
                return 0;
            }

            dot = 1;
        }
        else if (c == 'e' || c == 'E')
        {
            if (exp)
            {
                return 0;
            }

            if (nc == '+' || nc == '-')
            {
                if (nnc < '0' || nnc > '9')
                {
                    return 0;
                }
            }
            else if (nc < '0' || nc > '9')
            {
                return 0;
            }

            exp = prevExp = 1;
        }
        else
        {
            prevExp = 0;
        }

        s++;
    }

    return 1;
}

/***
 * Converts a `String` to a `Number`.
 * 
 * @arg strNum A string representing a valid number.
 * 
 * @returns A valid `Number` or `null`.
 */

static TByte fromStringFunc(VM* vm)
{
    melM_arg(vm, strNum, MELON_TYPE_STRING, 0);

    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* arrVal = melM_stackAllocRaw(&vm->stack);
    arrVal->type = MELON_TYPE_NUMBER;

    String* strObj = melM_strFromObj(strNum->pack.obj);
    const char* str = melM_strDataFromObj(strNum->pack.obj);

    if (!isNumber(str, strObj->len))
    {
        arrVal->type = MELON_TYPE_NULL;
        return 1;
    }

    arrVal->pack.value.number = strtod(str, NULL);

    return 1;
}

/***
 * Converts an `Integer` to a `Number` by using the rounding used by the C compiler
 * when casting `TInteger`s to `TNumber`s.
 * If the value to convert is already a `Number` the value is returned unmodified.
 * If the provided value was not an `Integer` or a `Number` an error is raised.
 * 
 * @arg val The value to be converted to a `Number`, either an `Integer` or a `Number`
 * 
 * @returns A valid `Number`.
 */

static TByte fromNumberFunc(VM* vm)
{
    melM_arg(vm, val, MELON_TYPE_NONE, 0);

   if (val->type == MELON_TYPE_NUMBER)
   {
       melM_stackPush(&vm->stack, val);
   }
   else if (val->type == MELON_TYPE_INTEGER)
   {
       Value n;
       n.type = MELON_TYPE_NUMBER;
       n.pack.value.number = val->pack.value.integer;

       melM_stackPush(&vm->stack, &n);
   }
   else
   {
       melM_fatal(vm, "Can't convert type '%s' to number", MELON_TYPES_NAMES[val->type]);
       return 0;
   }

    return 1;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "fromString", 1, 0, &fromStringFunc },
    { "fromNumber", 1, 0, &fromNumberFunc },
    { NULL, 1, 0, NULL }
};

TRet melNumberModuleInit(VM* vm)
{
    return melNewModule(vm, funcs);
}