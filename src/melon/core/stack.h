#ifndef __melon__stack_h__
#define __melon__stack_h__

#include "melon/core/types.h"
#include "melon/core/value.h"
#include "melon/core/stack_meta.h"

#include <stdint.h>

typedef Value StackEntry;
typedef struct GCItem GCItem;

melM_defineStackStruct(Stack, StackEntry);

#define melM_vstackPushGCItem(stackVar, item) \
    { \
        GCItem* itemTmp = item; \
        StackEntry* topEntry = melM_stackAllocRaw(stackVar); \
        topEntry->type = itemTmp->type; \
        topEntry->pack.obj = itemTmp; \
    } \
    (void)0

#define melM_vstackPushNull(stackVar) \
    melM_stackAllocRaw(stackVar)->type = MELON_TYPE_NULL

#endif // __melon__stack_h__