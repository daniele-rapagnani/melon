#include "melon/core/program.h"
#include "melon/core/vm.h"

#ifdef _TRACK_ALLOCATIONS_GC
#include <stdio.h>
#endif

GCItem* melNewProgram(VM* vm)
{
    TSize objSize = sizeof(Program);

    GCItem* obj = melNewGCItem(vm, objSize);
    obj->type = MELON_TYPE_PROGRAM;

#ifdef _TRACK_ALLOCATIONS_GC
    printf("Allocated program of size " MELON_PRINTF_SIZE " (%p), total bytes allocated = " MELON_PRINTF_SIZE "\n", objSize + sizeof(GCItem), obj, vm->gc.usedBytes);
#endif

    return obj;
}

TRet melFreeProgram(VM* vm, GCItem* item)
{
    TSize size = sizeof(Program);

#ifdef _TRACK_ALLOCATIONS_GC
    printf("Freeing program of " MELON_PRINTF_SIZE " bytes (%p), total bytes now = " MELON_PRINTF_SIZE "\n", size + sizeof(GCItem), item, vm->gc.usedBytes - (size + sizeof(GCItem)));
#endif

    vm->gc.usedBytes -= size;

    return melFreeGCItem(vm, item);
}