#ifndef __melon__value_h__
#define __melon__value_h__

#include "melon/core/types.h"
#include "melon/core/gc_item.h"

#define MELON_TYPE_NONE       0
#define MELON_TYPE_INTEGER    1
#define MELON_TYPE_NUMBER     2
#define MELON_TYPE_BOOL       3
#define MELON_TYPE_NULL       4
#define MELON_TYPE_NATIVEPTR  5
#define MELON_TYPE_SMALLRANGE 6
#define MELON_TYPE_NATIVEIT   7
#define MELON_TYPE_STRING     8
#define MELON_TYPE_OBJECT     9
#define MELON_TYPE_CLOSURE    10
#define MELON_TYPE_PROGRAM    11
#define MELON_TYPE_ARRAY      12
#define MELON_TYPE_SYMBOL     13
#define MELON_TYPE_RANGE      14
#define MELON_TYPE_FUNCTION   15

#define MELON_TYPE_MAX_BOXED 6
#define MELON_TYPE_MAX_ID    15

// The maximum number of character a 64 bit number can take
#define MELON_MAX_INT64_CHAR_COUNT 21
#define MELON_MAX_NUMBER_CHAR_COUNT 128

#define melM_isGCItem(x) ((x)->type > MELON_TYPE_MAX_BOXED)

#define melM_valFromTypeDec(funcName, typeName) \
    void funcName(Value* v, typeName val)

#define melM_valToNumber(vm, val, newVal) \
    Value newVal; \
    if (melToNumberValue(vm, val, &newVal) != 0) { \
        melM_fatal(vm, "Expected numeric value"); \
    }

typedef union UValue
{
    TInteger integer;
    TNumber number;
    TByte boolean;
} UValue;

typedef struct Value
{
    TType type;
    TByte unused[7];
    
    union
    {
        UValue value;
        GCItem* obj;
    } pack;
} Value;

typedef TSize TValueHash;

melM_valFromTypeDec(intVal, TInteger);
melM_valFromTypeDec(numVal, TNumber);

TRet melCmpValues(VM* vm, Value* a, Value* b, TRet* res);
TRet melGetHashValue(VM* vm, Value* v, TValueHash* hash);
TRet melToNumberValue(VM* vm, Value* v, Value* out);
TRet melVisitValue(VM* vm, Value* val, GCItemVisitor visit, void* ctx, TSize depth);

#endif // __melon__value_h__