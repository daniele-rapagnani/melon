#ifndef __melon__tstring_h__
#define __melon__tstring_h__

#include "melon/core/types.h"
#include "melon/core/gc_item.h"
#include "melon/core/vm.h"

#define melM_strFromObj(obj) ((String*)((TPtr)(obj) + sizeof(GCItem)))
#define melM_strDataFromObj(obj) ((char*)(melM_strFromObj(obj)->string))

typedef struct String
{
    TUint64 len;
    TUint64 hash;
    TBool internalized : 1;
    char* string;
} String;

GCItem* melNewString(VM* vm, const char* str, TSize size);
GCItem* melNewDataString(VM* vm, TSize size);
TRet melUpdateStringHash(VM* vm, GCItem* s);
GCItem* melNewStringFromStrings(VM* vm, GCItem* s1, GCItem* s2);
TRet melFreeString(VM* vm, GCItem* s);
TRet melCmpStrings(VM* vm, GCItem* s1, GCItem* s2);

TRet melFirstIndexOfString(
    VM* vm, 
    GCItem* haystack, 
    GCItem* needle, 
    TUint64 start, 
    TUint64* index
);

GCItem* melNewReplaceString(
    VM* vm, 
    GCItem* haystack, 
    GCItem* needle, 
    GCItem* replacement,
    TUint64* firstIdx,
    TUint64* lastIdx
);

#endif // __melon__tstring_h__