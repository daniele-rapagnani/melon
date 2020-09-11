#include "melon/core/config.h"
#include "melon/core/object.h"
#include "melon/core/tstring.h"
#include "melon/core/symbol.h"
#include "melon/core/gc.h"
#include "melon/core/vm.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef _TRACK_ALLOCATIONS_GC
#include <stdio.h>
#endif

Value MELON_SYMBOLIC_KEYS[MELON_OBJSYM_COUNT];

static TSymbolID MIN_OBJSYM_ID;
static TSymbolID MAX_OBJSYM_ID;

#define melM_getInternalSymbolMask(value) \
    (1 << (melM_symbolFromObj(value->pack.obj)->id - MIN_OBJSYM_ID))

#define melM_isInternalSymbol(value) \
    ( \
        value->type == MELON_TYPE_SYMBOL && \
        melM_symbolFromObj(value->pack.obj)->id >= MIN_OBJSYM_ID && \
        melM_symbolFromObj(value->pack.obj)->id <= MAX_OBJSYM_ID \
    )

#define melM_bucket(obj, hash) \
    ((hash) & ((obj)->capacity - 1))

TRet melInitSystemObject(VM* vm)
{
    melCreateGlobalSymbolKey(vm, "sum operator", &MELON_SYMBOLIC_KEYS[MELON_OBJSYM_ADD]);
    melCreateGlobalSymbolKey(vm, "sub operator", &MELON_SYMBOLIC_KEYS[MELON_OBJSYM_SUB]);
    melCreateGlobalSymbolKey(vm, "multiply operator", &MELON_SYMBOLIC_KEYS[MELON_OBJSYM_MUL]);
    melCreateGlobalSymbolKey(vm, "division operator", &MELON_SYMBOLIC_KEYS[MELON_OBJSYM_DIV]);
    melCreateGlobalSymbolKey(vm, "concat operator", &MELON_SYMBOLIC_KEYS[MELON_OBJSYM_CONCAT]);
    melCreateGlobalSymbolKey(vm, "comparison operator", &MELON_SYMBOLIC_KEYS[MELON_OBJSYM_CMP]);
    melCreateGlobalSymbolKey(vm, "indexed assignment operator", &MELON_SYMBOLIC_KEYS[MELON_OBJSYM_SETINDEX]);
    melCreateGlobalSymbolKey(vm, "indexed access operator", &MELON_SYMBOLIC_KEYS[MELON_OBJSYM_INDEX]);
    melCreateGlobalSymbolKey(vm, "property access operator", &MELON_SYMBOLIC_KEYS[MELON_OBJSYM_PROPERTY]);
    melCreateGlobalSymbolKey(vm, "property set operator", &MELON_SYMBOLIC_KEYS[MELON_OBJSYM_SETPROPERTY]);
    melCreateGlobalSymbolKey(vm, "negation operator", &MELON_SYMBOLIC_KEYS[MELON_OBJSYM_NEG]);
    melCreateGlobalSymbolKey(vm, "size operator", &MELON_SYMBOLIC_KEYS[MELON_OBJSYM_SIZEARR]);
    melCreateGlobalSymbolKey(vm, "hashing function", &MELON_SYMBOLIC_KEYS[MELON_OBJSYM_HASH]);
    melCreateGlobalSymbolKey(vm, "iterator function", &MELON_SYMBOLIC_KEYS[MELON_OBJSYM_ITERATOR]);
    melCreateGlobalSymbolKey(vm, "next function", &MELON_SYMBOLIC_KEYS[MELON_OBJSYM_NEXT]);
    melCreateGlobalSymbolKey(vm, "power operator", &MELON_SYMBOLIC_KEYS[MELON_OBJSYM_POW]);
    melCreateGlobalSymbolKey(vm, "call operator", &MELON_SYMBOLIC_KEYS[MELON_OBJSYM_CALL]);

    MIN_OBJSYM_ID = melM_symbolFromObj(MELON_SYMBOLIC_KEYS[MELON_OBJSYM_ADD].pack.obj)->id;
    MAX_OBJSYM_ID = melM_symbolFromObj(MELON_SYMBOLIC_KEYS[MELON_OBJSYM_CALL].pack.obj)->id;

    assert(MAX_OBJSYM_ID - MIN_OBJSYM_ID + 1 == MELON_OBJSYM_COUNT - 1);

    return 0;
}

GCItem* melNewObject(VM* vm)
{
    TSize objSize = sizeof(Object);

    GCItem* obj = melNewGCItem(vm, objSize);
    obj->type = MELON_TYPE_OBJECT;
    
    Object* objObj = melM_objectFromObj(obj);
    objObj->capacity = MELON_OBJ_MIN_CAPACITY;
    objObj->count = 0;
    objObj->prototype = NULL;
    objObj->internalSymbolsFlags = 0;
    objObj->freeCb = NULL;
    objObj->table = malloc(objObj->capacity * sizeof(struct ObjectNode*));

    memset(objObj->table, 0, objObj->capacity * sizeof(struct ObjectNode*));

#ifdef _TRACK_ALLOCATIONS_GC
    printf("Allocated obj of size " MELON_PRINTF_SIZE " (%p), total bytes allocated = " MELON_PRINTF_SIZE "\n", objSize + sizeof(GCItem), obj, vm->gc.usedBytes);
#endif

    return obj;
}

TRet melFreeObject(VM* vm, GCItem* obj)
{
    Object* objObj = melM_objectFromObj(obj);

    if (objObj->freeCb)
    {
        objObj->freeCb(vm, obj);
    }

    TSize size = sizeof(Object);

#ifdef _TRACK_ALLOCATIONS_GC
    printf("Freeing object of " MELON_PRINTF_SIZE " bytes (%p), total bytes now = " MELON_PRINTF_SIZE "\n", size + sizeof(GCItem), obj, vm->gc.usedBytes - (size + sizeof(GCItem)));
#endif

    vm->gc.usedBytes -= size;

    for(TSize i = 0; i < objObj->capacity; i++)
    {
        struct ObjectNode* curNode = objObj->table[i];
        struct ObjectNode* node;

        while (curNode != NULL)
        {
            node = curNode;
            curNode = curNode->next;

#ifdef _ZERO_MEMORY_ON_FREE_GC
            memset(node, 0, sizeof(struct ObjectNode));
#endif

            free(node);
        }
    }

    free(objObj->table);

    return melFreeGCItem(vm, obj);
}

#include <stdio.h>

static void resizeBuckets(VM* vm, Object* obj)
{
    //@TODO: run gc
    TSize oldCapacity = obj->capacity;
    obj->capacity <<= 1;
    obj->table = realloc(obj->table, sizeof(struct ObjectNode*) * obj->capacity);
    memset(obj->table + oldCapacity, 0, sizeof(struct ObjectNode*) * oldCapacity);

    TSize newBitMask = (obj->capacity - 1) & (~(oldCapacity - 1));
    TSize newBucket = 0;

    for(TSize i = 0; i < oldCapacity; i++)
    {
        struct ObjectNode* curNode = obj->table[i];
        struct ObjectNode* prevNode = NULL;

        while (curNode != NULL)
        {
            if (curNode->hash & newBitMask)
            {
                struct ObjectNode* movNode = curNode;
                newBucket = melM_bucket(obj, curNode->hash);

                if (prevNode != NULL)
                {
                    prevNode->next = movNode->next;
                }
                else
                {
                    obj->table[i] = movNode->next;
                }

                curNode = movNode->next;
                
                struct ObjectNode* curHead = obj->table[newBucket];
                obj->table[newBucket] = movNode;
                movNode->next = curHead;
            }
            else
            {
                prevNode = curNode;
                curNode = curNode->next;
            }
        }
    }
}

static void insertObjectNode(VM* vm, Object* obj, TSize bucket, Value* key, Value* value, TValueHash hash)
{
    struct ObjectNode* node = malloc(sizeof(struct ObjectNode));
    assert(node != NULL);
    
    node->key = *key;
    node->value = *value;
    node->hash = hash;

    struct ObjectNode* curHead = obj->table[bucket];
    obj->table[bucket] = node;
    node->next = curHead;

    obj->count++;

    if (obj->count >= obj->capacity)
    {
        resizeBuckets(vm, obj);
    }
}

static TRet compareKey(VM* vm, Value* key, Value* other)
{
    if (
        key->type == MELON_TYPE_OBJECT 
        && melM_objectHasCustomSymbol(key->pack.obj, MELON_OBJSYM_CMP)
    )
    {
        melM_stackPush(&vm->stack, key);
        melM_stackPush(&vm->stack, other);

        if (melPerformCustomOpVM(vm, MELON_OBJSYM_CMP, 2) == 0)
        {
            Value result = *melM_stackPop(&vm->stack);

            // @todo: This code is shared with the vm
            if (result.type != MELON_TYPE_INTEGER)
            {
                melM_fatal(vm, "The custom equality operator must return an integer");
                return 1;
            }

            return result.pack.value.integer;
        }
        else
        {
            melM_stackPopCount(&vm->stack, 2);
        }
    }

    TRet res = -1;
    melCmpValues(vm, key, other, &res);

    return res;
}

static struct ObjectNode* checkValue(VM* vm, Object* obj, TSize bucket, Value* key, TSize hash)
{
    struct ObjectNode* head = obj->table[bucket];

    while (head != NULL)
    {
        if (
            head->key.type == MELON_TYPE_STRING
            && key->type == MELON_TYPE_STRING
            && melM_strFromObj(head->key.pack.obj)->string == melM_strFromObj(key->pack.obj)->string
        )
        {
            return head;
        }
        else if (head->hash == hash && compareKey(vm, key, &head->key) == 0)
        {
            return head;
        }

        head = head->next;
    }

    return NULL;
}

Value* melGetValueObject(VM* vm, GCItem* obj, Value* key)
{
    Object* objObj = melM_objectFromObj(obj);
    TValueHash hash = 0;
    
    if (melGetHashValue(vm, key, &hash) != 0)
    {
        return NULL;
    }

    TSize bucket = melM_bucket(objObj, hash);
    struct ObjectNode* entry = checkValue(vm, objObj, bucket, key, hash);

    return entry != NULL ? &entry->value : NULL;
}

Value* melResolveValueObject(VM* vm, GCItem* obj, Value* key)
{
    assert(obj->type == MELON_TYPE_OBJECT);

    Value* val = melGetValueObject(vm, obj, key);

    if (val != NULL)
    {
        return val;
    }

    Object* objObj = melM_objectFromObj(obj);

    if (objObj->prototype == NULL)
    {
        return NULL;
    }

    return melResolveValueObject(vm, objObj->prototype, key);
}

TRet melSetPrototypeObject(VM* vm , GCItem* obj, GCItem* prototype)
{
    assert(obj != NULL);
    assert(obj->type == MELON_TYPE_OBJECT);

    assert(prototype != NULL);
    assert(prototype->type = MELON_TYPE_OBJECT);

    Object* objObj = melM_objectFromObj(obj);
    objObj->prototype = prototype;

    melWriteBarrierGC(vm, obj, prototype);

    return 0;
}

GCItem* melGetPrototypeObject(VM* vm , GCItem* obj)
{
    assert(obj != NULL);
    assert(obj->type == MELON_TYPE_OBJECT);

    Object* objObj = melM_objectFromObj(obj);

    return objObj->prototype;
}

TRet melSetOperatorObject(VM* vm, GCItem* obj, MelonObjectSymbols op, Value* operatorClosure)
{
    assert(obj != NULL);
    assert(obj->type == MELON_TYPE_OBJECT);
    assert(op < MELON_OBJSYM_COUNT);
    assert(operatorClosure != NULL);
    assert(operatorClosure->type == MELON_TYPE_CLOSURE);

    return melSetValueObject(vm, obj, &MELON_SYMBOLIC_KEYS[op], operatorClosure);
}

GCItem* melResolveSymbolObject(VM* vm, GCItem* obj, MelonObjectSymbols op)
{
    assert(obj != NULL);
    assert(obj->type == MELON_TYPE_OBJECT);
    assert(op < MELON_OBJSYM_COUNT);

    if (!melM_objectHasCustomSymbol(obj, op))
    {
        return NULL;
    }
    
    Value* opFuncVal = melResolveValueObject(vm, obj, &MELON_SYMBOLIC_KEYS[op]);
    return opFuncVal != NULL ? opFuncVal->pack.obj : NULL;
}

TRet melRemoveKeyObject(VM* vm, GCItem* obj, Value* key)
{
    Object* objObj = melM_objectFromObj(obj);
    TValueHash hash = 0;
    
    if (melGetHashValue(vm, key, &hash) != 0)
    {
        return 1;
    }

    TSize bucket = melM_bucket(objObj, hash);

    struct ObjectNode* head = objObj->table[bucket];
    struct ObjectNode** prev = &objObj->table[bucket];

    while (head != NULL)
    {
        if (compareKey(vm, key, &head->key) == 0)
        {
            if (melM_isInternalSymbol(key))
            {
                melM_clearBits(
                    objObj->internalSymbolsFlags, 
                    melM_getInternalSymbolMask(key)
                );
            }

            *prev = head->next;
            
            assert(objObj > 0);
            objObj->count--;

#ifdef _ZERO_MEMORY_ON_FREE_GC
            memset(head, 0, sizeof(struct ObjectNode));
#endif

            free(head);

            return 0;
        }

        prev = &head->next;
        head = head->next;
    }

    return 1;
}

Value* melGetKeyAtIndexObject(VM* vm, GCItem* obj, TSize index, TBool withSymbols)
{
    //@todo: Performance is probably bad here.
    //       This all the more true if we want to access
    //       the items sequentially

    Object* objObj = melM_objectFromObj(obj);

    if (index >= objObj->count)
    {
        return NULL;
    }

    for (TSize i = 0; i < objObj->capacity; i++)
    {
        struct ObjectNode* head = objObj->table[i];

        while (head != NULL)
        {
            if (withSymbols || head->key.type != MELON_TYPE_SYMBOL)
            {
                if (index == 0)
                {
                    return &head->key;
                }

                index--;
            }
            
            head = head->next;
        }
    }

    assert(!withSymbols);
    return NULL;
}

TRet melSetValueObject(VM* vm, GCItem* obj, Value* key, Value* value)
{
    Object* objObj = melM_objectFromObj(obj);
    TValueHash hash = 0;
    
    if (melGetHashValue(vm, key, &hash) != 0)
    {
        return 1;
    }

    if (melM_isInternalSymbol(key))
    {
        melM_setBits(
            objObj->internalSymbolsFlags,
            melM_getInternalSymbolMask(key)
        );
    }

    TSize bucket = hash % objObj->capacity;

    struct ObjectNode* entry = checkValue(vm, objObj, bucket, key, hash);

    melWriteBarrierValueGC(vm, obj, value);

    if (entry)
    {
        entry->value = *value;
        return 0;
    }

    insertObjectNode(vm, objObj, bucket, key, value, hash);
    melWriteBarrierValueGC(vm, obj, key);

    return 0;
}

GCItem* melCloneObject(VM* vm, GCItem* obj, TBool deep)
{
    TBool paused = 0;

    if (melIsPausedGC(vm, &vm->gc) != 0)
    {
        paused = 1;
        melSaveAndPauseGC(vm, &vm->gc);
    }

    Object* objObj = melM_objectFromObj(obj);
    GCItem* dstObj = melNewObject(vm);
    
    for (TSize i = 0; i < objObj->capacity; i++)
    {
        struct ObjectNode* head = objObj->table[i];

        while (head != NULL)
        {
            Value val = head->value;

            if (deep && val.type == MELON_TYPE_OBJECT)
            {
                val.pack.obj = melCloneObject(vm, val.pack.obj, deep);
            }

            melSetValueObject(
                vm,
                dstObj,
                &head->key,
                &val
            );
            
            head = head->next;
        }
    }

    if (paused)
    {
        melRestorePauseGC(vm, &vm->gc);
    }

    return dstObj;
}

TRet melMergeObject(VM* vm, GCItem* target, GCItem* with, TBool deep)
{
    Object* objObj = melM_objectFromObj(target);
    Object* srcObjObj = melM_objectFromObj(with);

    for (TSize i = 0; i < objObj->capacity; i++)
    {
        struct ObjectNode* head = objObj->table[i];

        while (head != NULL)
        {
            Value* mergeVal = melGetValueObject(vm, with, &head->key);

            if (mergeVal != NULL)
            {
                if (
                    deep && 
                    mergeVal->type == MELON_TYPE_OBJECT && 
                    head->value.type == MELON_TYPE_OBJECT
                )
                {
                    if (melMergeObject(vm, head->value.pack.obj, mergeVal->pack.obj, deep) != 0)
                    {
                        return 1;
                    }
                }
                else
                {
                    head->value = *mergeVal;
                }
            }
            
            head = head->next;
        }
    }

    // @TODO: This can probably be optimized by saving the keys
    //        we already visited in the step above.

    for (TSize i = 0; i < srcObjObj->capacity; i++)
    {
        struct ObjectNode* head = srcObjObj->table[i];

        while (head != NULL)
        {
            if (melGetValueObject(vm, target, &head->key) == NULL)
            {
                melSetValueObject(vm, target, &head->key, &head->value);
            }
            
            head = head->next;
        }
    }

    return 0;
}