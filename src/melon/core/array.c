#include "melon/core/array.h"
#include "melon/core/vm.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef _TRACK_ALLOCATIONS_GC
#include <stdio.h>
#endif

GCItem* melNewArray(VM* vm)
{
    TSize objSize = sizeof(Array);

    GCItem* arr = melNewGCItem(vm, objSize);
    arr->type = MELON_TYPE_ARRAY;
    
    Array* arrObj = melM_arrayFromObj(arr);
    arrObj->capacity = 0;
    arrObj->count = 0;
    arrObj->data = NULL;

#ifdef _TRACK_ALLOCATIONS_GC
    printf("Allocated array of size " MELON_PRINTF_SIZE " (%p), total bytes allocated = " MELON_PRINTF_SIZE "\n", objSize, arr, vm->gc.usedBytes);
#endif

    return arr;
}

GCItem* melNewArrayFromArray(VM* vm, GCItem* array, TInteger* start, TInteger* end)
{
    GCItem* newArr = melNewArray(vm);
    Array* arrayObj = melM_arrayFromObj(array);

    if (arrayObj->count == 0)
    {
        return newArr;
    }

    TInteger startIdx = start != NULL ? *start : 0;
    TInteger endIdx = end != NULL ? *end : arrayObj->count - 1;

    if (startIdx < 0)
    {
        startIdx = arrayObj->count + startIdx;
    }

    if (endIdx < 0)
    {
        endIdx = arrayObj->count + endIdx;
    }

    if (startIdx >= arrayObj->count || endIdx >= arrayObj->count || endIdx < startIdx)
    {
        return newArr;
    }

    TSize itemsCount = (endIdx + 1) - startIdx;

    if (melEnsureSizeArray(vm, newArr, itemsCount) != 0)
    {
        return NULL;
    }

    char* startPtr = (char*)arrayObj->data + (startIdx * sizeof(Value));
    TSize itemsByteSize = itemsCount * sizeof(Value);

    Array* newArrObj = melM_arrayFromObj(newArr);
    memcpy(newArrObj->data, startPtr, itemsByteSize);
    newArrObj->count = itemsCount;

    return newArr;
}

GCItem* melNewArrayFromArrays(VM* vm, GCItem* a, GCItem* b)
{
    GCItem* newArr = melNewArray(vm);
    Array* aObj = melM_arrayFromObj(a);
    Array* bObj = melM_arrayFromObj(b);
    Array* newArrObj = melM_arrayFromObj(newArr);

    TSize total = aObj->count + bObj->count;

    if (melEnsureSizeArray(vm, newArr, total) != 0)
    {
        return NULL;
    }

    newArrObj->count = total;

    if (aObj->count > 0)
    {
        memcpy(newArrObj->data, aObj->data, aObj->count * sizeof(Value));
    }

    if (bObj->count > 0)
    {
        memcpy(newArrObj->data + aObj->count, bObj->data, bObj->count * sizeof(Value));
    }

    return newArr;
}

TRet melFreeArray(VM* vm, GCItem* arr)
{
    Array* arrObj = melM_arrayFromObj(arr);

    TSize size = sizeof(Array)
        + (arrObj->capacity * sizeof(Value))
    ;

#ifdef _TRACK_ALLOCATIONS_GC
    printf("Freeing array of " MELON_PRINTF_SIZE " bytes (%p), total bytes now = " MELON_PRINTF_SIZE "\n", size + sizeof(GCItem), arr, vm->gc.usedBytes - (size + sizeof(GCItem)));
#endif

    vm->gc.usedBytes -= size;

    if (arrObj->data != NULL)
    {
#ifdef _ZERO_MEMORY_ON_FREE_GC
        memset(arrObj->data, 0, size - sizeof(Array));
#endif

        free(arrObj->data);
    }

    return melFreeGCItem(vm, arr);
}

Value* melGetIndexArray(VM* vm, GCItem* arr, ArraySize idx)
{
    Array* arrObj = melM_arrayFromObj(arr);

    if (idx >= arrObj->count)
    {
        return NULL;
    }

    return &arrObj->data[idx];
}

TRet melSetIndexArray(VM* vm, GCItem* arr, ArraySize idx, Value* value)
{
    Array* arrObj = melM_arrayFromObj(arr);

    if (idx >= arrObj->count)
    {
        return 1;
    }

    arrObj->data[idx] = *value;

    melWriteBarrierValueGC(vm, arr, value);

    return 0;
}

TRet melEnsureSizeArray(VM* vm, GCItem* arr, ArraySize size)
{
    Array* arrObj = melM_arrayFromObj(arr);

    if (arrObj->capacity < size)
    {
        arrObj->capacity = size * 2;
        arrObj->data = realloc(arrObj->data, sizeof(Value) * arrObj->capacity);
        assert(arrObj->data != NULL);
    }

    return 0;
}

TRet melPushArray(VM* vm, GCItem* arr, Value* value)
{
    Array* arrObj = melM_arrayFromObj(arr);

    if (melEnsureSizeArray(vm, arr, arrObj->count + 1) != 0)
    {
        return 1;
    }

    arrObj->count++;

    return melSetIndexArray(vm, arr, arrObj->count - 1, value);
}

TRet melRemoveRangeArray(VM* vm, GCItem* arr, ArraySize idxStart, ArraySize idxEnd)
{
    Array* arrObj = melM_arrayFromObj(arr);

    if (
        (arrObj->count <= idxStart || arrObj->count <= idxEnd)
        || (idxStart > idxEnd)
    )
    {
        return 1;
    }

    ArraySize delSize = idxEnd - idxStart + 1;

    if (idxEnd < arrObj->count - 1)
    {
        memmove(
            &arrObj->data[idxStart], 
            &arrObj->data[idxEnd + 1],
            (arrObj->count - idxEnd) * sizeof(Value)
        );
    }
    
    arrObj->count -= delSize;

    return 0;
}