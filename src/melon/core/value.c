#include "melon/core/value.h"
#include "melon/core/tstring.h"
#include "melon/core/closure.h"
#include "melon/core/symbol.h"
#include "melon/core/vm.h"
#include "melon/core/object.h"

#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define melM_valFromTypeDef(funcName, typeName, typeEnum, varName) \
    melM_valFromTypeDec(funcName, typeName) \
    { \
        v->type = typeEnum; \
        v->pack.value.varName = val; \
    }

melM_valFromTypeDef(intVal, TInteger, MELON_TYPE_INTEGER, integer);
melM_valFromTypeDef(numVal, TNumber, MELON_TYPE_NUMBER, number);

TRet melCmpValues(VM* vm, Value* a, Value* b, TRet* res)
{
    if (a->type == MELON_TYPE_NULL || b->type == MELON_TYPE_NULL)
    {
        *res = a->type == MELON_TYPE_NULL && b->type == MELON_TYPE_NULL ? 0 : 1;
        return 0;
    }

    if (a->type != b->type)
    {
        return 1;
    }

    switch(a->type)
    {
        case MELON_TYPE_INTEGER:
            //@todo: This is in order to avoid overflows with TRet
            //       using more than a TByte for this seems an overkill
            //       but avoiding it may have performance overheads.
            //       The impact of this solution should be measured.
            
            *res = (a->pack.value.integer > b->pack.value.integer) 
                - (a->pack.value.integer < b->pack.value.integer)
            ;
            return 0;

        case MELON_TYPE_NUMBER:
            *res = 
                (a->pack.value.number > b->pack.value.number) 
                - (a->pack.value.number < b->pack.value.number)
            ;
            return 0;

        case MELON_TYPE_BOOL:
            *res = a->pack.value.boolean == b->pack.value.boolean ? 0 : 1;
            return 0;

        case MELON_TYPE_STRING:
            *res = melCmpStrings(vm, a->pack.obj, b->pack.obj);
            return 0;

        case MELON_TYPE_SYMBOL:
            *res = melCmpSymbols(vm, a->pack.obj, b->pack.obj);
            return 0;

        case MELON_TYPE_OBJECT:
        case MELON_TYPE_CLOSURE:
        case MELON_TYPE_PROGRAM:
            *res = !(a->pack.obj == b->pack.obj);
            return 0;
    }

    assert(0 /* Comparing unknown types */);
    return 1;
}

static void melGetObjectHashValue(VM* vm, Value* v, TUint64* hash)
{
    assert(v->type == MELON_TYPE_OBJECT);

    GCItem* hasher = melResolveSymbolObject(vm, v->pack.obj, MELON_OBJSYM_HASH);

    if (hasher == NULL)
    {
        *hash = (TPtr)v->pack.obj;
        return;
    }

    if (hasher->type != MELON_TYPE_CLOSURE)
    {
        melM_fatal(vm, "The hashing function is not a function, it's a '%s'", MELON_TYPES_NAMES[v->type]);
        return;
    }

    Closure* cl = melM_closureFromObj(hasher);
    Function* fn = melM_functionFromObj(cl->fn);

    melM_vstackPushGCItem(&vm->stack, hasher);
    melM_stackPush(&vm->stack, v);

    //@todo: Review this method stuff, I'm not convinced.
    melCallClosureSyncVM(vm, 1 - (fn->method ? 0 : 1), 0, 1);

    Value result = *melM_stackPop(&vm->stack);

    if (result.type != MELON_TYPE_INTEGER)
    {
        melM_fatal(
            vm, 
            "The hashing function must return an 'integer'. It returned a '%s'", 
            MELON_TYPES_NAMES[result.type]
        );

        return;
    }

    *hash = result.pack.value.integer;
}

TRet melGetHashValue(VM* vm, Value* v, TUint64* hash)
{
    if (v->type == MELON_TYPE_NULL)
    {
        return 1;
    }

    switch(v->type)
    {
        case MELON_TYPE_NUMBER:
            *hash = round(v->pack.value.number);
            return 0;

        case MELON_TYPE_INTEGER:
            *hash = v->pack.value.integer;
            return 0;

        case MELON_TYPE_BOOL:
            *hash = v->pack.value.boolean;
            return 0;

        case MELON_TYPE_STRING:
        {
            String* str = melM_strFromObj(v->pack.obj);
            *hash = str->hash;
            return 0;
        }

        case MELON_TYPE_SYMBOL:
        {
            Symbol* sym = melM_symbolFromObj(v->pack.obj);
            *hash = sym->id;
            return 0;
        }

        case MELON_TYPE_OBJECT:
            melGetObjectHashValue(vm, v, hash);
            return 0;

        case MELON_TYPE_CLOSURE:
        case MELON_TYPE_PROGRAM:
            *hash = (TPtr)v->pack.obj;
            return 0;
    }

    assert(0 /* Don't know how to hash value*/);
    return 1;
}

TRet melToNumberValue(VM* vm, Value* v, Value* out)
{
    if (v->type != MELON_TYPE_INTEGER && v->type != MELON_TYPE_NUMBER)
    {
        return 1;
    }

    out->type = MELON_TYPE_NUMBER;
    out->pack.value.number = (
        v->type == MELON_TYPE_INTEGER ? v->pack.value.integer : v->pack.value.number
    );

    return 0;
}

TRet melVisitValue(VM* vm, Value* val, GCItemVisitor visit, void* ctx, TSize depth)
{
    if (!melM_isGCItem(val))
    {
        return 0;
    }

    return melVisitGCItem(vm, val->pack.obj, visit, ctx, depth);
}