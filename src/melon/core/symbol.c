#include "melon/core/symbol.h"

#include <stdlib.h>
#include <assert.h>

#ifdef _TRACK_ALLOCATIONS_GC
#include <stdio.h>
#endif

GCItem* melNewSymbol(VM* vm, GCItem* label)
{
    GCItem* obj = melNewGCItem(vm, sizeof(Symbol));
    obj->type = MELON_TYPE_SYMBOL;
    
    Symbol* symbolObj = melM_symbolFromObj(obj);
    symbolObj->id = vm->nextSymbolId;
    symbolObj->label = label;

    if (vm->nextSymbolId == MELON_MAX_SYMBOL_ID)
    {
        melM_fatal(
            vm, 
            "You can't generate any more symbols. (you generated %lld)", 
            MELON_MAX_SYMBOL_ID
        );

        return NULL;
    }
    
    vm->nextSymbolId++;

#ifdef _TRACK_ALLOCATIONS_GC
    printf("Allocated symbol of size %llu (%p), total bytes allocated = %llu\n", sizeof(Symbol) + sizeof(GCItem), obj, vm->gc.usedBytes);
#endif

    return obj;
}

TRet melCmpSymbols(VM* vm, GCItem* a, GCItem* b)
{
    assert(a->type == MELON_TYPE_SYMBOL);
    assert(b->type == MELON_TYPE_SYMBOL);

    Symbol* sa = melM_symbolFromObj(a);
    Symbol* sb = melM_symbolFromObj(b);

    return !(sa->id == sb->id);
}

TRet melFreeSymbol(VM* vm, GCItem* s)
{
    TSize size = sizeof(Symbol);

#ifdef _TRACK_ALLOCATIONS_GC
    printf("Freeing symbol of %llu bytes (%p), total bytes now = %llu\n", size + sizeof(GCItem), s, vm->gc.usedBytes - (size + sizeof(GCItem)));
#endif

    vm->gc.usedBytes -= size;

    return melFreeGCItem(vm, s);
}