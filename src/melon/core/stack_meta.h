#ifndef __melon__stack_meta_h__
#define __melon__stack_meta_h__

#include "melon/core/types.h"
#include "melon/core/memory_utils.h"

#include <assert.h>
#include <string.h>

typedef TSize StackSize;

TRet melCreateStack(void* stack, StackSize minSize, TSize itemSize);
TRet melResizeStack(void* stack, StackSize newSize);

#define melM_stackSetTop(stackVar, newTop) \
    (stackVar)->top = newTop

#define melM_stackPop(stackVar) \
    (&((stackVar)->stack[((stackVar)->top--) - 1]))

#define melM_stackPopCount(stackVar, count) \
    (stackVar)->top -= count

#define melM_stackGet(stackVar, index) \
    (&((stackVar)->stack[(index)]))

#define melM_stackSet(stackVar, index, var) \
    (stackVar)->stack[(index)] = *var

#define melM_stackOffset(stackVar, offset) \
    (&((stackVar)->stack[(stackVar)->top - (offset) - 1]))

#define melM_stackTop(stackVar) \
    (&((stackVar)->stack[(stackVar)->top - 1]))

#define melM_stackIsEmpty(stackVar) \
    ((stackVar)->top == 0)

#define melM_stackAllocRaw(stackVar) \
    (&((stackVar)->stack[(stackVar)->top++]))

#define melM_stackEnsure(stackVar, newSize) \
    if ((stackVar)->size < newSize) \
    { \
        melResizeStack((stackVar), newSize * 2); \
    } \
    (void)0

#define melM_stackEnsureSave(stackVar, newSize, save) \
    { \
        TSize saveIdx = (TSize)(save - (stackVar)->stack); \
        melM_stackEnsure((stackVar), newSize); \
        save = (stackVar)->stack + saveIdx; \
    } \
    (void)0

#define melM_stackInsert(stackVar, beforeOffset, val) \
    assert(beforeOffset > 0); \
    (stackVar)->top++; \
    memmove( \
        &(stackVar)->stack[(stackVar)->top - beforeOffset], \
        &(stackVar)->stack[(stackVar)->top - beforeOffset - 1], \
        beforeOffset * ((stackVar)->itemSize) \
    ); \
    (stackVar)->stack[(stackVar)->top - beforeOffset - 1] = *val

#define melM_stackPushRaw(stackVar, val) \
    (stackVar)->stack[(stackVar)->top++] = *(val)
    
#define melM_stackPush(stackVar, val) \
    melM_stackEnsure(stackVar, (stackVar)->top + 1); \
    melM_stackPushRaw(stackVar, val)

#define melM_stackFree(stackVar) \
    melResizeStack((stackVar), 0)

#define melM_defineStackStruct(StackType, StackEntryType) \
    typedef struct StackType                                                  \
    {                                                                         \
        StackSize top;                                                        \
        StackSize size;                                                       \
        TSize itemSize; \
        StackEntryType *stack;                                                \
        TRet (*stackInvalidatedFunc)(void* context, void* oldStack);                              \
        void *stackInvalidatedCtx;                                            \
    } StackType;

#endif // __melon__stack_meta_h__