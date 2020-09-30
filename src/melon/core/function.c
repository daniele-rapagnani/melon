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
    printf("Allocated function of size " MELON_PRINTF_SIZE " (%p), total bytes allocated = " MELON_PRINTF_SIZE "\n", objSize + sizeof(GCItem), obj, vm->gc.usedBytes);
#endif

    return obj;
}

TRet melFreeFunction(VM* vm, GCItem* item)
{
    TSize size = sizeof(Function);
    
#ifdef _TRACK_ALLOCATIONS_GC
    printf("Freeing function of " MELON_PRINTF_SIZE " bytes (%p), total bytes now = " MELON_PRINTF_SIZE "\n", size + sizeof(GCItem), item, vm->gc.usedBytes - (size + sizeof(GCItem)));

    Function* f = melM_functionFromObj(item);

    if (f->debug.file != NULL) 
    {
        printf(
            "Function was declared at: %s:" MELON_PRINTF_SIZE "\n",
            f->debug.file,
            f->debug.count > 0 ? f->debug.lines[0] : 0
        );
    }
#endif

    vm->gc.usedBytes -= size;

    return melFreeGCItem(vm, item);
}