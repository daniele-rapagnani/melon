#ifndef __melon__array_h__
#define __melon__array_h__

#include "melon/core/types.h"
#include "melon/core/value.h"
#include "melon/core/gc_item.h"

#define melM_arrayFromObj(obj) ((Array*)((TPtr)(obj) + sizeof(GCItem)))

typedef TSize ArraySize;

typedef struct Array
{
    ArraySize capacity;
    ArraySize count;
    Value* data;
} Array;

GCItem* melNewArray(VM* vm);
GCItem* melNewArrayFromArray(VM* vm, GCItem* array, TInteger* start, TInteger* end);
GCItem* melNewArrayFromArrays(VM* vm, GCItem* a, GCItem* b);
Value* melGetIndexArray(VM* vm, GCItem* arr, ArraySize idx);
TRet melSetIndexArray(VM* vm, GCItem* arr, ArraySize idx, Value* value);
TRet melRemoveRangeArray(VM* vm, GCItem* arr, ArraySize idxStart, ArraySize idxEnd);
TRet melEnsureSizeArray(VM* vm, GCItem* arr, ArraySize size);
TRet melPushArray(VM* vm, GCItem* arr, Value* value);
TRet melFreeArray(VM* vm, GCItem* arr);

#endif // __melon__array_h__