#ifndef __melon__symbol_h__
#define __melon__symbol_h__

#include "melon/core/types.h"
#include "melon/core/gc_item.h"
#include "melon/core/vm.h"

#define melM_symbolFromObj(obj) ((Symbol*)((TPtr)(obj) + sizeof(GCItem)))

typedef struct Symbol
{
    TSymbolID id;
    GCItem* label;
} Symbol;

GCItem* melNewSymbol(VM* vm, GCItem* label);
TRet melCmpSymbols(VM* vm, GCItem* a, GCItem* b);
TRet melFreeSymbol(VM* vm, GCItem* s);

#endif // __melon__symbol_h__