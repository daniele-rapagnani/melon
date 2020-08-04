#ifndef __melon__gc_item_h__
#define __melon__gc_item_h__

#include "melon/core/types.h"
#include "melon/core/config.h"
#include "melon/core/macros.h"

#ifdef _TRACK_ALLOCATIONS_GC
#define MELON_GC_STATE_FREED 0x3
#endif

#define melM_getColorGCItem(gc, i) ((i)->gcState.color)
#define melM_setColorGCItem(gc, i, c) ((i)->gcState.color = (c))

#define melM_isWhiteGCItem(gc, i) (melM_getColorGCItem(gc, i) == (gc)->whiteMarker)
#define melM_isGreyGCItem(gc, i) (melM_getColorGCItem(gc, i) == (gc)->greyMarker)
#define melM_isBlackGCItem(gc, i) (melM_getColorGCItem(gc, i) == (gc)->blackMarker)
#define melM_isDarkGCItem(gc, i) (melM_isBlackGCItem(gc, i) || melM_isGreyGCItem(gc, i))

#define melM_getAgeGCItem(gc, i) ((i)->gcState.generation)
#define melM_setAgeGCItem(gc, i, g) ((i)->gcState.generation = (g))
#define melM_isOldGCItem(gc, i) (melM_getAgeGCItem(gc, i) == (MELON_GC_AGE_MAX + 1))
#define melM_isOlderThanGCItem(gc, a, b) \
    (melM_isOldGCItem(gc, a) && !melM_isOldGCItem(gc, b))

struct VM;
typedef struct VM VM;

typedef struct GCItemState
{
    TByte color : 2;
    TByte generation : 6;
} GCItemState;

typedef struct GCItem
{
    TType type;
#ifdef _ZERO_MEMORY_ON_FREE_GC
    TSize size;
#endif
    struct GCItem* next;
    struct GCItem* prev;
    GCItemState gcState;
} GCItem;

typedef TRet(*GCItemVisitor)(VM*, void*, GCItem*, TSize depth);

GCItem* melNewGCItem(VM* vm, TSize totalSize);
TRet melFreeGCItem(VM* vm, GCItem* item);
TRet melVisitGCItem(VM* vm, GCItem* root, GCItemVisitor visit, void* ctx, TSize depth);

#endif // __melon__gc_item_h__