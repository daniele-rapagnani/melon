#include "melon/core/closure.h"
#include "melon/core/function.h"
#include "melon/core/vm.h"

#include <assert.h>

#ifdef _TRACK_ALLOCATIONS_GC
#include <stdio.h>
#endif

GCItem* melNewClosure(VM* vm, GCItem* fnObj)
{
    assert(fnObj->type == MELON_TYPE_FUNCTION);
    Function* fn = melM_functionFromObj(fnObj);

    TSize objSize = sizeof(Closure) + (sizeof(Upvalue*) * fn->upvaluesInfos.count);

    GCItem* obj = 
    melNewGCItem(vm, objSize);
    obj->type = MELON_TYPE_CLOSURE;
    
    Closure* clObj = melM_closureFromObj(obj);
    clObj->fn = fnObj;

    melWriteBarrierGC(vm, obj, fnObj);

#ifdef _TRACK_ALLOCATIONS_GC
    printf("Allocated closure of size %llu (%p), total bytes allocated = %llu\n", objSize + sizeof(GCItem), obj, vm->gc.usedBytes);
#endif

    return obj;
}

TRet melFreeClosure(VM* vm, GCItem* item)
{
    Closure* clObj = melM_closureFromObj(item);
    Function* fn = melM_functionFromObj(clObj->fn);

    TSize size = sizeof(Closure) + (sizeof(Upvalue*) * fn->upvaluesInfos.count);

#ifdef _TRACK_ALLOCATIONS_GC
    printf("Freeing closure of %llu bytes (%p), total bytes now = %llu\n", size + sizeof(GCItem), item, vm->gc.usedBytes - (size + sizeof(GCItem)));
#endif

    vm->gc.usedBytes -= size;

    Upvalue** upvalues = melM_closureUpvaluesFromObj(item);

    for (TSize i = 0; i < fn->upvaluesInfos.count; i++)
    {
        melReleaseUpvalueVM(vm, upvalues[i]);
    }

    return melFreeGCItem(vm, item);
}