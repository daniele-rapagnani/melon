#ifndef __melon__native_iterator_h__
#define __melon__native_iterator_h__

#include "melon/core/vm.h"

typedef struct NativeIterator NativeIterator;

typedef TRet(*NativeIteratorNext)(VM*, NativeIterator*);

#define melM_nativeIteratorFields() \
    Value value; \
    GCItem* result; \
    Value* done; \
    Value* next; \
    TSize size; \
    NativeIteratorNext nextFunc

typedef struct NativeIterator
{
    melM_nativeIteratorFields();
} NativeIterator;

typedef struct NativeIndexedIterator
{
    melM_nativeIteratorFields();
    TSize index;
} NativeIndexedIterator;

#define melM_nativeitFromObj(obj) ((NativeIterator*)((TPtr)(obj) + sizeof(GCItem)))
#define melM_nativeitToObj(obj) ((GCItem*)((TPtr)(obj) - sizeof(GCItem)))

GCItem* melNewNativeIterator(VM* vm, const Value* value, NativeIteratorNext nextFunc, TSize size);
TRet melFreeNativeIterator(VM* vm, GCItem* nit);

#endif // __melon__native_iterator_h__