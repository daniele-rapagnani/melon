#ifndef __melon__object_h__
#define __melon__object_h__

#include "melon/core/types.h"
#include "melon/core/value.h"
#include "melon/core/gc_item.h"
#include "melon/core/macros.h"

#define melM_objectFromObj(obj) ((Object*)((TPtr)(obj) + sizeof(GCItem)))

typedef void(*ObjectFreeCallback)(VM*, GCItem*);

struct ObjectNode
{
    Value key;
    Value value;
    TValueHash hash;
    struct ObjectNode* next;
};

typedef struct Object
{
    TSize capacity;
    TSize count;
    struct ObjectNode** table;
    TUint32 internalSymbolsFlags;
    GCItem* prototype;
    ObjectFreeCallback freeCb;
} Object;

typedef enum MelonObjectSymbols
{
    MELON_OBJSYM_NONE,
    MELON_OBJSYM_ADD,
    MELON_OBJSYM_SUB,
    MELON_OBJSYM_MUL,
    MELON_OBJSYM_DIV,
    MELON_OBJSYM_CONCAT,
    MELON_OBJSYM_CMP,
    MELON_OBJSYM_INDEX,
    MELON_OBJSYM_SETINDEX,
    MELON_OBJSYM_PROPERTY,
    MELON_OBJSYM_SETPROPERTY,
    MELON_OBJSYM_NEG,
    MELON_OBJSYM_SIZEARR,
    MELON_OBJSYM_HASH,
    MELON_OBJSYM_ITERATOR,
    MELON_OBJSYM_NEXT,
    MELON_OBJSYM_POW,
    MELON_OBJSYM_CALL,
    MELON_OBJSYM_COUNT,
} MelonObjectSymbols;

extern Value MELON_SYMBOLIC_KEYS[MELON_OBJSYM_COUNT];

#define melM_objectHasCustomSymbol(gcitem, symbol) \
    (\
        melM_objectFromObj(gcitem)->prototype != NULL \
        || melM_checkBits(melM_objectFromObj(gcitem)->internalSymbolsFlags, 1 << ((symbol) - 1)) \
    )

TRet melInitSystemObject(VM* vm);

GCItem* melNewObject(VM* vm);
Value* melGetValueObject(VM* vm, GCItem* obj, Value* key);
Value* melResolveValueObject(VM* vm, GCItem* obj, Value* key);
TRet melSetPrototypeObject(VM* vm , GCItem* obj, GCItem* prototype);
GCItem* melGetPrototypeObject(VM* vm , GCItem* obj);
GCItem* melResolveSymbolObject(VM* vm, GCItem* obj, MelonObjectSymbols op);
TRet melSetValueObject(VM* vm, GCItem* obj, Value* key, Value* value);
TRet melRemoveKeyObject(VM* vm, GCItem* obj, Value* key);
Value* melGetKeyAtIndexObject(VM* vm, GCItem* obj, TSize index, TBool withSymbols);
GCItem* melCloneObject(VM* vm, GCItem* obj, TBool deep);
TRet melMergeObject(VM* vm, GCItem* target, GCItem* with, TBool deep);
TRet melFreeObject(VM* vm, GCItem* obj);

#endif // __melon__object_h__