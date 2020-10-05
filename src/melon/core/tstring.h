#ifndef __melon__tstring_h__
#define __melon__tstring_h__

#include "melon/core/types.h"
#include "melon/core/gc_item.h"
#include "melon/core/vm.h"

#define melM_strFromObj(obj) ((String*)((TPtr)(obj) + sizeof(GCItem)))
#define melM_strDataFromObj(obj) ((char*)(melM_strFromObj(obj)->string))

typedef struct String
{
    TSize len;
    TValueHash hash;
    TBool internalized : 1;
    char* string;
} String;

GCItem* melNewString(VM* vm, const char* str, TSize size);
GCItem* melNewDataString(VM* vm, TSize size);
TRet melUpdateStringHash(VM* vm, GCItem* s);
GCItem* melNewStringFromString(VM* vm, GCItem* s1, TInteger* start, TInteger* end);
GCItem* melNewStringFromStrings(VM* vm, GCItem* s1, GCItem* s2);
TRet melFreeString(VM* vm, GCItem* s);
TRet melCmpStrings(VM* vm, GCItem* s1, GCItem* s2);

TRet melFirstIndexOfString(
    VM* vm, 
    GCItem* haystack, 
    GCItem* needle, 
    TSize start, 
    TSize* index
);

GCItem* melNewReplaceString(
    VM* vm, 
    GCItem* haystack, 
    GCItem* needle, 
    GCItem* replacement,
    TSize* firstIdx,
    TSize* lastIdx
);

#endif // __melon__tstring_h__