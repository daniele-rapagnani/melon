#include "melon/core/native_iterator.h"

#ifdef _TRACK_ALLOCATIONS_GC
#include <stdio.h>
#endif

GCItem* melNewNativeIterator(VM* vm, const Value* value, NativeIteratorNext nextFunc, TSize size)
{
    GCItem* obj = melNewGCItem(vm, size);
    obj->type = MELON_TYPE_NATIVEIT;
    
    NativeIterator* nitObj = melM_nativeitFromObj(obj);
    nitObj->value = *value;
    nitObj->size = size;
    nitObj->nextFunc = nextFunc;
    nitObj->result = NULL;
    nitObj->done = NULL;
    nitObj->next = NULL;

    melWriteBarrierValueGC(vm, obj, &nitObj->value);

#ifdef _TRACK_ALLOCATIONS_GC
    printf("Allocated native iterator of size " MELON_PRINTF_SIZE " (%p), total bytes allocated = " MELON_PRINTF_SIZE "\n", size + sizeof(GCItem), obj, vm->gc.usedBytes);
#endif

    return obj;
}

TRet melFreeNativeIterator(VM* vm, GCItem* nit)
{
    NativeIterator* nitObj = melM_nativeitFromObj(nit);

#ifdef _TRACK_ALLOCATIONS_GC
    printf("Freeing symbol of " MELON_PRINTF_SIZE " bytes (%p), total bytes now = " MELON_PRINTF_SIZE "\n", nitObj->size + sizeof(GCItem), nit, vm->gc.usedBytes - (nitObj->size + sizeof(GCItem)));
#endif

    vm->gc.usedBytes -= nitObj->size;

    return melFreeGCItem(vm, nit);
}