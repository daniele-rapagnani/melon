#include "melon/core/stack_meta.h"
#include "melon/core/memory_utils.h"

#include <stdlib.h>

melM_defineStackStruct(BaseStack, void)

TRet melCreateStack(void* stack, StackSize minSize, TSize itemSize)
{
    BaseStack* bs = ((BaseStack*)stack);

    bs->stack = malloc(minSize * itemSize);
    assert(bs->stack != NULL);

    if (bs->stack == NULL)
    {
        bs->stack = NULL;
        return 1;
    }

    bs->itemSize = itemSize;
    bs->size = minSize;
    bs->top = 0;
    bs->stackInvalidatedFunc = NULL;
    bs->stackInvalidatedCtx = NULL;

    return 0;                    
}

TRet melResizeStack(void* stack, StackSize newSize)
{
    BaseStack* bs = ((BaseStack*)stack);

    void *oldStack = bs->stack;

    if (melResizeBuffer((void **)&bs->stack, &bs->size, bs->itemSize, newSize) != 0)
    {
        return 1;
    }

    if (oldStack != bs->stack)
    {
        if (
            bs->stackInvalidatedFunc != NULL 
            && bs->stackInvalidatedFunc(bs->stackInvalidatedCtx, oldStack) != 0
        )
        {
            return 1;
        }
    }

    return 0;
}