#include "range.h"

#include <stdlib.h>
#include <assert.h>

#ifdef _TRACK_ALLOCATIONS_GC
#include <stdio.h>
#endif

GCItem* melNewRange(VM* vm, TInteger start, TInteger end)
{
    GCItem* obj = melNewGCItem(vm, sizeof(Range));
    obj->type = MELON_TYPE_RANGE;
    
    Range* rangeObj = melM_rangeFromObj(obj);
    rangeObj->start = start;
    rangeObj->end = end;

#ifdef _TRACK_ALLOCATIONS_GC
    printf("Allocated range of size " MELON_PRINTF_SIZE " (%p), total bytes allocated = " MELON_PRINTF_SIZE "\n", sizeof(Range) + sizeof(GCItem), obj, vm->gc.usedBytes);
#endif

    return obj;
}

TRet melFreeRange(VM* vm, GCItem* item)
{
    TSize size = sizeof(Range);

#ifdef _TRACK_ALLOCATIONS_GC
    printf("Freeing range of " MELON_PRINTF_SIZE " bytes (%p), total bytes now = " MELON_PRINTF_SIZE "\n", size + sizeof(GCItem), item, vm->gc.usedBytes - (size + sizeof(GCItem)));
#endif

    vm->gc.usedBytes -= size;

    return melFreeGCItem(vm, item);
}