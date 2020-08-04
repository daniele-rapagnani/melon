#ifndef __range_h__
#define __range_h__

#include "melon/core/types.h"
#include "melon/core/gc_item.h"
#include "melon/core/vm.h"
#include "melon/core/macros.h"

#define melM_rangeFromObj(obj) ((Range*)((TPtr)(obj) + sizeof(GCItem)))

#define MELON_RANGE_SMALL_BITS (sizeof(TInteger) * 8 / 2)
#define MELON_RANGE_SMALL_MASK (melM_maxBitsValue(MELON_RANGE_SMALL_BITS))
#define MELON_RANGE_SMALL_MIN ((MELON_RANGE_SMALL_MASK / 2) - 1)
#define MELON_RANGE_SMALL_MAX (MELON_RANGE_SMALL_MASK / 2)

#define melM_rangePointToUnsigned(n) \
    ((TUInteger)((n) + MELON_RANGE_SMALL_MAX))

#define melM_rangePointIsSmall(n) \
    (melM_rangePointToUnsigned(n) >= 0 && melM_rangePointToUnsigned(n) <= MELON_RANGE_SMALL_MASK)

#define melM_rangeIsSmall(a, b) \
    ( \
        melM_rangePointIsSmall(a) \
        && melM_rangePointIsSmall(b) \
    )

#define melM_rangeSmallGet(a, b) \
    (melM_rangePointToUnsigned(a) | (melM_rangePointToUnsigned(b) << MELON_RANGE_SMALL_BITS))

#define melM_rangeSmallStart(n) \
    ((((n)->pack.value.integer) & MELON_RANGE_SMALL_MASK) - MELON_RANGE_SMALL_MAX)

#define melM_rangeSmallEnd(n) \
    (((((n)->pack.value.integer) >> MELON_RANGE_SMALL_BITS) & MELON_RANGE_SMALL_MASK) - MELON_RANGE_SMALL_MAX)

typedef struct Range
{
    TInteger start;
    TInteger end;
} Range;

GCItem* melNewRange(VM* vm, TInteger start, TInteger end);
TRet melFreeRange(VM* vm, GCItem* s);

#endif // __range_h__