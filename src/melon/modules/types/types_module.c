#include "melon/modules/types/types_module.h"

#include "melon/modules/modules.h"
#include "melon/core/closure.h"
#include "melon/core/tstring.h"

/***
 * @module
 * 
 * This module provides functions to extract type-related information
 * from values.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define melM_defineCheckType(typeName) \
    static TByte is##typeName(VM* vm) \
    { \
        Value *input = melM_stackOffset(&vm->stack, 0);              \
        Value res; \
        res.type = MELON_TYPE_BOOL; \
        \
        res.pack.value.boolean = \
            (input != NULL && input->type == MELON_TYPE_##typeName) ? 1 : 0 \
        ; \
        \
        melM_stackPush(&vm->stack, &res); \
        return 1; \
    }

#define melM_defineModuleFunc(funcName, typeName) \
    { #funcName, 1, 0, is##typeName },

/***
 * @function isInteger
 * 
 * Checks whether a given value is an `Integer`.
 * @arg val The value to check
 * @returns `true` if `val` is an `Integer`, `false` otherwise.
 */

melM_defineCheckType(INTEGER)

/***
 * @function isNumber
 * 
 * Checks whether a given value is a `Number`.
 * @arg val The value to check
 * @returns `true` if `val` is a `Number`, `false` otherwise.
 */

melM_defineCheckType(NUMBER)

/***
 * @function isBool
 * 
 * Checks whether a given value is a `Bool`.
 * @arg val The value to check
 * @returns `true` if `val` is a `Bool`, `false` otherwise.
 */

melM_defineCheckType(BOOL)

/***
 * @function isNull
 * 
 * Checks whether a given value is `null`.
 * @arg val The value to check
 * @returns `true` if `val` is `null`, `false` otherwise.
 */

melM_defineCheckType(NULL)

/***
 * @function isString
 * 
 * Checks whether a given value is a `String`.
 * @arg val The value to check
 * @returns `true` if `val` is a `String`, `false` otherwise.
 */

melM_defineCheckType(STRING)

/***
 * @function isObject
 * 
 * Checks whether a given value is an `Object`.
 * @arg val The value to check
 * @returns `true` if `val` is an `Object`, `false` otherwise.
 */

melM_defineCheckType(OBJECT)

/***
 * @function isClosure
 * 
 * Checks whether a given value is a `Function`.
 * @arg val The value to check
 * @returns `true` if `val` is a `Function`, `false` otherwise.
 */

melM_defineCheckType(CLOSURE)

/***
 * @function isProgram
 * 
 * Checks whether a given value is a `Program`.
 * @arg val The value to check
 * @returns `true` if `val` is a `Program`, `false` otherwise.
 */

melM_defineCheckType(PROGRAM)

/***
 * @function isArray
 * 
 * Checks whether a given value is an `Array`.
 * @arg val The value to check
 * @returns `true` if `val` is an `Array`, `false` otherwise.
 */

melM_defineCheckType(ARRAY)

TByte ofFunc(VM* vm)
{
    melM_arg(vm, val, MELON_TYPE_NONE, 0);

    melM_vstackPushGCItem(
        &vm->stack, 
        melNewString(vm, MELON_TYPES_NAMES[val->type], strlen(MELON_TYPES_NAMES[val->type]))
    );

    return 1;
}

/***
 * @function of
 * 
 * Returns a string representation of the type of a value.
 * 
 * @arg val The value to inspect
 * @returns A string representation of the type of the value
 */

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    /*** @funcdef isInteger */
    melM_defineModuleFunc(isInteger, INTEGER)
    /*** @funcdef isNumber */
    melM_defineModuleFunc(isNumber, NUMBER)
    /*** @funcdef isBool */
    melM_defineModuleFunc(isBool, BOOL)
    /*** @funcdef isNull */
    melM_defineModuleFunc(isNull, NULL)
    /*** @funcdef isString */
    melM_defineModuleFunc(isString, STRING)
    /*** @funcdef isObject */
    melM_defineModuleFunc(isObject, OBJECT)
    /*** @funcdef isClosure */
    melM_defineModuleFunc(isClosure, CLOSURE)
    /*** @funcdef isProgram */
    melM_defineModuleFunc(isProgram, PROGRAM)
    /*** @funcdef isArray */
    melM_defineModuleFunc(isArray, ARRAY)
    { "of", 1, 0, ofFunc },
    { NULL, 0, 0, NULL }
};

TRet melTypesModuleInit(VM* vm)
{
    return melNewModule(vm, funcs);
}