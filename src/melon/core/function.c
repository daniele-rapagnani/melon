#include "melon/core/function.h"
#include "melon/core/vm.h"

#include <string.h>

#ifdef _TRACK_ALLOCATIONS_GC
#include <stdio.h>
#endif

GCItem* melNewFunction(VM* vm)
{
    TSize objSize = sizeof(Function);

    GCItem* obj = melNewGCItem(vm, objSize);
    obj->type = MELON_TYPE_FUNCTION;
    
    Function* fn = melM_functionFromObj(obj);
    memset(fn, 0, sizeof(Function));

#ifdef _TRACK_ALLOCATIONS_GC
    printf("Allocated function of size %llu (%p), total bytes allocated = %llu\n", objSize + sizeof(GCItem), obj, vm->gc.usedBytes);
#endif

    return obj;
}

TRet melFreeFunction(VM* vm, GCItem* item)
{
    TSize size = sizeof(Function);
    
#ifdef _TRACK_ALLOCATIONS_GC
    printf("Freeing function of %llu bytes (%p), total bytes now = %llu\n", size + sizeof(GCItem), item, vm->gc.usedBytes - (size + sizeof(GCItem)));
#endif

    vm->gc.usedBytes -= size;

    return melFreeGCItem(vm, item);
}