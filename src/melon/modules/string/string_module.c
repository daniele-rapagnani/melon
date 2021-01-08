#include "melon/modules/string/string_module.h"
#include "melon/modules/string/format.h"

#include "melon/modules/modules.h"
#include "melon/core/closure.h"
#include "melon/core/symbol.h"
#include "melon/core/range.h"
#include "melon/core/tstring.h"
#include "melon/core/array.h"
#include "melon/core/function.h"
#include "melon/core/utils.h"

/***
 * @module
 * 
 * This module provides functions for basic string manipulation.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

TRet melToString(VM* vm, const Value* se)
{
    GCItem* newString = NULL;

    switch(se->type)
    {
        case MELON_TYPE_BOOL:
            {
                const char* c = se->pack.value.boolean ? "true" : "false";
                newString = melNewString(vm, c, strlen(c));
            }
            break;

        case MELON_TYPE_STRING:
            newString = se->pack.obj;
            break;

        case MELON_TYPE_NATIVEPTR:
            {
                static char number[MELON_MAX_INT64_CHAR_COUNT + 2];
                snprintf(number, MELON_MAX_INT64_CHAR_COUNT + 2, "%p", (void*)se->pack.value.integer);
                newString = melNewString(vm, number, strlen(number));
            }
            break;

        case MELON_TYPE_NATIVEIT:
            {
                static char number[MELON_MAX_INT64_CHAR_COUNT + 5];
                snprintf(number, MELON_MAX_INT64_CHAR_COUNT + 5, "it %p", (void*)se->pack.value.integer);
                newString = melNewString(vm, number, strlen(number));
            }
            break;

        case MELON_TYPE_NUMBER:
            {
                static char number[MELON_MAX_NUMBER_CHAR_COUNT];
                snprintf(number, MELON_MAX_NUMBER_CHAR_COUNT, "%.9f", se->pack.value.number);
                newString = melNewString(vm, number, strlen(number));
            }
            break;

        case MELON_TYPE_INTEGER:
            {
                static char number[MELON_MAX_INT64_CHAR_COUNT];
                snprintf(
                    number, 
                    MELON_MAX_INT64_CHAR_COUNT, 
#ifdef MELON_64BIT
                    "" MELON_PRINTF_INT "",
#else
                    "%ld",
#endif
                    se->pack.value.integer
                );
                newString = melNewString(vm, number, strlen(number));
            }
            break;

        case MELON_TYPE_OBJECT:
            newString = melNewString(vm, "[Object]", strlen("[Object]"));
            break;

        case MELON_TYPE_ARRAY:
            newString = melNewString(vm, "[Array]", strlen("[Array]"));
            break;

        case MELON_TYPE_CLOSURE:
        {
            Closure* c = melM_closureFromObj(se->pack.obj);
            Function* fn = melM_functionFromObj(c->fn);
            String* s = fn->name != NULL ? melM_strFromObj(fn->name) : NULL;
            const char* sn = fn->name != NULL ? melM_strDataFromObj(fn->name) : "@anonymous@"; 
            
            static char funcName[MELON_VM_MAX_FUNC_NAME];

            snprintf(
                funcName, 
                MELON_VM_MAX_FUNC_NAME, 
                "[Closure %.*s%s]", 
                s != NULL ? s->len : strlen("@anonymous@"), 
                sn,
                fn->native != NULL ? " (native)" : ""
            );

            newString = melNewString(vm, funcName, strlen(funcName));
        }
            break;

        case MELON_TYPE_PROGRAM:
            newString = melNewString(vm, "[Program]", strlen("[Program]"));
            break;

        case MELON_TYPE_SYMBOL:
        {
            Symbol* s = melM_symbolFromObj(se->pack.obj);
            
            if (s->label == NULL)
            {
                newString = melNewString(vm, "[Symbol]", strlen("[Symbol]"));
                break;
            }

            String* sl = melM_strFromObj(s->label);
            const char* slStr = melM_strDataFromObj(s->label);

            static char symName[MELON_VM_MAX_SYMBOL_NAME];

            snprintf(
                symName, 
                MELON_VM_MAX_SYMBOL_NAME, 
                "[Symbol \"%.*s\"]", 
                sl->len, 
                slStr
            );

            newString = melNewString(vm, symName, strlen(symName));
        }
            break;

        case MELON_TYPE_RANGE:
        {
            Range* r = melM_rangeFromObj(se->pack.obj);

            static char rangeName[MELON_VM_MAX_RANGE_STRING];

            snprintf(
                rangeName, 
                MELON_VM_MAX_RANGE_STRING, 
                "[Range start = %d, end = %d]", 
                r->start, 
                r->end
            );

            newString = melNewString(vm, rangeName, strlen(rangeName));
        }
            break;

        case MELON_TYPE_SMALLRANGE:
        {
            static char rangeName[MELON_VM_MAX_RANGE_STRING];

            snprintf(
                rangeName, 
                MELON_VM_MAX_RANGE_STRING, 
                "[Small Range start = %d, end = %d]", 
                melM_rangeSmallStart(se),
                melM_rangeSmallEnd(se)
            );

            newString = melNewString(vm, rangeName, strlen(rangeName));
        }
            break;

        case MELON_TYPE_FUNCTION:
        {
            Function* fn = melM_functionFromObj(se->pack.obj);
            String* s = fn->name != NULL ? melM_strFromObj(fn->name) : NULL;  
            const char* sn = fn->name != NULL ? melM_strDataFromObj(fn->name) : "@anonymous@"; 
            
            static char funcName[MELON_VM_MAX_FUNC_NAME];

            snprintf(
                funcName, 
                MELON_VM_MAX_FUNC_NAME, 
                "[Function %.*s%s]", 
                s != NULL ? s->len : strlen("@anonymous@"), 
                sn,
                fn->native != NULL ? " (native)" : ""
            );

            newString = melNewString(vm, funcName, strlen(funcName));
        }
            break;

        case MELON_TYPE_NULL:
            newString = melNewString(vm, "null", strlen("null"));
            break;

        default:
            assert(0);
    }

    melM_vstackPushGCItem(&vm->stack, newString);

    return 0;
}

/***
 * Converts the provided value to its `String` representation.
 * 
 * @arg val The value to convert to string
 * @returns The string representation of `val`
 */

static TByte toString(VM* vm)
{
    Value* se = melGetLocalVM(vm, melGetTopCallFrameVM(vm), 0, 0);
    melToString(vm, se);
    return 1;
}

/***
 * Returns an `Integer` representing the ASCII code of the
 * character at index `idx` in `str`.
 * 
 * @arg str The subject string
 * @arg idx The character's index
 * 
 * @returns An `Integer` or `null` if the index is out of range
 */

static TByte charCodeAt(VM* vm)
{
    melM_arg(vm, str, MELON_TYPE_STRING, 0);
    melM_arg(vm, idx, MELON_TYPE_INTEGER, 1);

    String* strObj = melM_strFromObj(str->pack.obj);
    const char* strData = melM_strDataFromObj(str->pack.obj);

    Value resultValue;
    
    if (idx->pack.value.integer < strObj->len)
    {
        resultValue.type = MELON_TYPE_INTEGER;
        resultValue.pack.value.integer = (TUint8)strData[idx->pack.value.integer];
    }
    else
    {
        resultValue.type = MELON_TYPE_NULL;
    }
    
    melM_stackPush(&vm->stack, &resultValue);
    return 1;
}

/***
 * Creates a string from a list of ASCII codes.
 * 
 * @arg codes A single `Integer` or an `Array` of `Integer` values. Any non-`Integer` entry will be skipped.
 * @returns A new string made up of the provided sequence of ASCII codes
 */

static TByte fromCharCodes(VM* vm)
{
    Value* codes = melGetLocalVM(vm, melGetTopCallFrameVM(vm), 0, 0);

    if (codes->type != MELON_TYPE_ARRAY && codes->type != MELON_TYPE_INTEGER)
    {
        melM_fatal(vm, "Expected an array of ASCII codes or a single integer.");
        return 0;
    }

    TSize len = 1;

    if (codes->type == MELON_TYPE_ARRAY)
    {
        Array* arr = melM_arrayFromObj(codes->pack.obj);
        len = arr->count;
    }

    GCItem* str = melNewDataString(vm, len + 1);
    melM_vstackPushGCItem(&vm->stack, str);

    char* strData = melM_strDataFromObj(str);

    if (codes->type == MELON_TYPE_ARRAY)
    {
        Array* arr = melM_arrayFromObj(codes->pack.obj);

        for (TSize i = 0; i < arr->count; i++)
        {
            Value* code = melGetIndexArray(vm, codes->pack.obj, i);

            if (code->type != MELON_TYPE_INTEGER)
            {
                strData[i] = 0;
                continue;
            }

            strData[i] = code->pack.value.integer & 0xFF;
        }
    }
    else
    {
        strData[0] = codes->pack.value.integer & 0xFF;
    }

    strData[len] = '\0';
    melUpdateStringHash(vm, str);

    return 1;
}

/***
 * Looks for any occurrence of `needle` in `haystack` starting from the `start` index.
 * 
 * @arg haystack The string in which to find the `needle`
 * @arg needle The substring to search for
 * @arg ?start The index at which the search should start, defaults to 0
 * 
 * @returns An `Integer` with the index in `haystack` at which the first character of `needle` was found. `null` if `needle` couldn't be found.
 */

static TByte find(VM* vm)
{
    melM_arg(vm, haystack, MELON_TYPE_STRING, 0);
    melM_arg(vm, needle, MELON_TYPE_STRING, 1);
    melM_argOptional(vm, start, MELON_TYPE_INTEGER, 2);

    TSize index;
    TSize startIndex = start->type == MELON_TYPE_NULL ? 0 : start->pack.value.integer;
    Value result;

    if (melFirstIndexOfString(vm, haystack->pack.obj, needle->pack.obj, startIndex, &index) == 0)
    {
        result.type = MELON_TYPE_INTEGER;
        result.pack.value.integer = index;
    }
    else
    {
        result.type = MELON_TYPE_NULL;
    }

    melM_stackPush(&vm->stack, &result);

    return 1;
}

/***
 * Replaces with `replacement` any occurrence of `needle` found inside `haystack` between the
 * `start` (inclusive) and `end` (exclusive) indices.
 * 
 * @arg haystack The string in which to find the `needle`
 * @arg needle The substring to be replaced by `replacement`
 * @arg replacement The string to replace any occurrence of `needle`
 * @arg ?start The index of `haystack` at which the search should start, defaults to 0
 * @arg ?end The index of `haystack` at which the search should end, defaults to the length of `haystack`
 * 
 * @returns A new string with `needle` replaced by `replacement` or the original `haystack` if `needle` was not found.
 */

static TByte replace(VM* vm)
{
    melM_arg(vm, haystack, MELON_TYPE_STRING, 0);
    melM_arg(vm, needle, MELON_TYPE_STRING, 1);
    melM_arg(vm, replacement, MELON_TYPE_STRING, 2);
    melM_argOptional(vm, start, MELON_TYPE_INTEGER, 3);
    melM_argOptional(vm, end, MELON_TYPE_INTEGER, 4);

    TSize index;
    TSize startIndex = start->type == MELON_TYPE_NULL ? 0 : start->pack.value.integer;
    TSize endIndex = end->type == MELON_TYPE_NULL ? 0 : end->pack.value.integer;

    GCItem* newStr = melNewReplaceString(
        vm,
        haystack->pack.obj,
        needle->pack.obj,
        replacement->pack.obj,
        start->type != MELON_TYPE_NULL ? &startIndex : NULL,
        end->type != MELON_TYPE_NULL ? &endIndex : NULL
    );

    melM_vstackPushGCItem(&vm->stack, newStr);

    return 1;
}

static TByte transform(VM* vm, int(*transformFunc)(int), TSize limit)
{
    melM_arg(vm, str, MELON_TYPE_STRING, 0);

    String* strObj = melM_strFromObj(str->pack.obj);
    char* strData = melM_strDataFromObj(str->pack.obj);

    GCItem* newStr = melNewString(vm, strData, strObj->len);
    melM_vstackPushGCItem(&vm->stack, newStr);
    char* resData = melM_strDataFromObj(newStr);

    for (TSize i = 0; i < strObj->len; i++)
    {
        if (limit > 0 && i >= limit)
        {
            break;
        }

        resData[i] = (char)transformFunc(resData[i]);
    }

    return 1;
}

/***
 * Transforms the provided string to lowercase.
 * 
 * @arg str The string to transform
 * @returns The transformed string
 */

static TByte toLower(VM* vm)
{
    return transform(vm, tolower, 0);
}

/***
 * Transforms the provided string to uppercase.
 * 
 * @arg str The string to transform
 * @returns The transformed string
 */

static TByte toUpper(VM* vm)
{
    return transform(vm, toupper, 0);
}

/***
 * Capitalizes the provided string, 
 * only the first character is transformed to uppercase.
 * 
 * @arg str The string to transform
 * @returns The transformed string
 */

static TByte toCapitalized(VM* vm)
{
    return transform(vm, toupper, 1);
}

/***
 * Trims a string removing any space character from 
 * the beginning and end of a string.
 * 
 * @arg str The string to be trimmed
 * @returns The trimmed string
 */

static TByte trim(VM* vm)
{
    melM_arg(vm, str, MELON_TYPE_STRING, 0);

    String* strObj = melM_strFromObj(str->pack.obj);
    char* strData = melM_strDataFromObj(str->pack.obj);

    TSize frontSpacesCount = 0;
    TSize backSpacesCount = 0;

    for (TSize i = 0; i < strObj->len; i++)
    {
        if (!isspace(strData[i]))
        {
            break;
        }

        frontSpacesCount++;
    }

    for (TSize i = strObj->len; i > frontSpacesCount + 1; i--)
    {
        if (!isspace(strData[i - 1]))
        {
            break;
        }

        backSpacesCount++;
    }

    assert((frontSpacesCount + backSpacesCount) <= strObj->len);

    // +1 for the null-termination
    TSize totalLen = strObj->len - frontSpacesCount - backSpacesCount + 1;
    GCItem* newStr = melNewDataString(vm, totalLen);

    melM_vstackPushGCItem(&vm->stack, newStr);

    char* resData = melM_strDataFromObj(newStr);
    TSize len = melM_strFromObj(newStr)->len;

    if (len > 0)
    {
        memcpy(resData, &strData[frontSpacesCount], len);
    }

    return 1;
}

/***
 * Formats a string with [printf](http://www.cplusplus.com/reference/cstdio/printf/) style
 * formatting.
 * 
 * @arg fmt The format to be used when formatting the string
 * @arg args An array of the values required by the `fmt` string, it may be an empty array
 * 
 * @returns The values provided in `args` formatted using `fmt`
 */

static TByte format(VM* vm)
{
    melM_arg(vm, fmt, MELON_TYPE_STRING, 0);
    melM_arg(vm, args, MELON_TYPE_ARRAY, 1);

    melM_vstackPushNull(&vm->stack);
    Value* res = melM_stackTop(&vm->stack);
    
    if (melFormat(vm, res, fmt->pack.obj, args->pack.obj) != 0)
    {
        res->type = MELON_TYPE_NULL;
    }

    return 1;
}

static void addSplitItem(VM* vm, GCItem* arr, const char* start, TSize len)
{
    if (len == 0)
    {
        return;
    }

    GCItem* item = melNewString(vm, start, len);
    
    Value itemV;
    itemV.type = item->type;
    itemV.pack.obj = item;

    melPushArray(vm, arr, &itemV);
}

/***
 * Splits `str` using `token` as a delimiter.
 * 
 * @arg str The string to be splitted
 * @arg token The substring to use as delimiter
 * 
 * @returns An array with the strings resulted from splitting `str` by `token`.
 */

static TByte split(VM* vm)
{
    melM_arg(vm, str, MELON_TYPE_STRING, 0);
    melM_arg(vm, token, MELON_TYPE_STRING, 1);

    String* strObj = melM_strFromObj(str->pack.obj);
    String* tokenObj = melM_strFromObj(token->pack.obj);

    GCItem* result = melNewArray(vm);
    melM_vstackPushGCItem(&vm->stack, result);

    if (tokenObj->len >= strObj->len)
    {
        return 1;
    }

    TSize ti = 0;
    TSize len = 0;
    const char* s = strObj->string;

    for (TSize i = 0; i < strObj->len; i++)
    {
        if (strObj->string[i] == tokenObj->string[ti])
        {
            ti++;

            if (ti == tokenObj->len)
            {             
                addSplitItem(vm, result, s, len);

                s += len + ti;
                ti = 0;
                len = 0;
            }

            continue;
        }

        len += ti + 1;
        ti = 0;
    }

    addSplitItem(vm, result, s, len);

    return 1;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "toString", 1, 0, toString },
    { "charCodeAt", 2, 0, charCodeAt },
    { "fromCharCodes", 1, 0, fromCharCodes },
    { "replace", 5, 0, replace },
    { "find", 3, 0, find },
    { "toLower", 1, 0, toLower },
    { "toUpper", 1, 0, toUpper },
    { "toCapitalized", 1, 0, toCapitalized },
    { "trim", 1, 0, trim },
    { "format", 2, 0, format, 0, 1 },
    { "split", 2, 0, split },
    { NULL, 0, 0, NULL }
};

TRet melStringModuleInit(VM* vm)
{
    return melNewModule(vm, funcs);
}