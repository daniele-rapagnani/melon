#include "melon/modules/array/array_module.h"
#include "melon/modules/modules.h"
#include "melon/core/array.h"
#include "melon/core/tstring.h"

/***
 * @module
 * 
 * This module defines utility functions for [`Array`](array.md) values manipulation.
 */

#include <stdlib.h>
#include <assert.h>

/***
 * Ensures the size of an array is at least the one provided.
 * 
 * @arg arr The array to resize
 * @arg size An integer with the new size
 */

static TByte resizeFunc(VM* vm)
{
    melM_arg(vm, arr, MELON_TYPE_ARRAY, 0);
    melM_arg(vm, size, MELON_TYPE_INTEGER, 1);

    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* arrVal = melM_stackAllocRaw(&vm->stack);
    arrVal->type = MELON_TYPE_BOOL;
    arrVal->pack.value.boolean = melEnsureSizeArray(vm, arr->pack.obj, size->pack.value.integer) == 0;

    Array* arrObj = melM_arrayFromObj(arr->pack.obj);
    TSize oldCount = arrObj->count;
    arrObj->count = size->pack.value.integer;

    memset(arrObj->data, MELON_TYPE_NULL, sizeof(Value) * arrObj->count);

    return 1;
}

/***
 * Returns the value stored at a given index for a given array without rising any error
 * if the index does not exist.
 * 
 * @arg arr The array
 * @arg index The index to lookup
 * 
 * @return The requeste value if present, `null` otherwise
 */

static TByte lookupFunc(VM* vm)
{
    melM_arg(vm, arr, MELON_TYPE_ARRAY, 0);
    melM_arg(vm, index, MELON_TYPE_INTEGER, 1);

    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* arrVal = melM_stackAllocRaw(&vm->stack);
    arrVal->type = MELON_TYPE_NULL;
    
    Value* idxVal = melGetIndexArray(vm, arr->pack.obj, index->pack.value.integer);

    if (idxVal != NULL)
    {
        *arrVal = *idxVal;
    }

    return 1;
}

/***
 * Removes one or more elements from an array, resizing it if necessary.
 * 
 * @arg arr The array
 * @arg start The index of the first element to remove
 * @arg end The index of the last element to remove
 * 
 * @returns `true` on success, `false` otherwise
 */

static TByte deleteFunc(VM* vm)
{
    melM_arg(vm, arr, MELON_TYPE_ARRAY, 0);
    melM_arg(vm, start, MELON_TYPE_INTEGER, 1);
    melM_arg(vm, end, MELON_TYPE_NONE, 2);

    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* result = melM_stackAllocRaw(&vm->stack);
    result->type = MELON_TYPE_BOOL;

    ArraySize startIdx = start->pack.value.integer;
    ArraySize endIdx = startIdx;

    if (end->type == MELON_TYPE_INTEGER)
    {
        endIdx = end->pack.value.integer;
    }
    
    result->pack.value.boolean = melRemoveRangeArray(
        vm, 
        arr->pack.obj,
        startIdx,
        endIdx
    ) == 0;

    return 1;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "resize", 2, 0, &resizeFunc },
    { "lookup", 2, 0, &lookupFunc },
    { "delete", 3, 0, &deleteFunc },
    { NULL, 0, 0, NULL }
};

TRet melArrayModuleInit(VM* vm)
{
    return melNewModule(vm, funcs);
}