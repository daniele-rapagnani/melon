#include "melon/core/gc_item.h"
#include "melon/core/gc.h"
#include "melon/core/object.h"
#include "melon/core/array.h"
#include "melon/core/closure.h"
#include "melon/core/tstring.h"
#include "melon/core/symbol.h"
#include "melon/core/range.h"
#include "melon/core/program.h"
#include "melon/core/native_iterator.h"
#include "melon/core/vm.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

GCItem* melNewGCItem(VM* vm, TSize totalSize)
{
    melTriggerGC(vm, &vm->gc);

    TSize objectSize = sizeof(GCItem) + totalSize;
    GCItem* newObj = malloc(objectSize);

#ifdef _ZERO_MEMORY_ON_FREE_GC
    newObj->size = objectSize;
#endif

    if (newObj == NULL)
    {
        melM_fatal(vm, "Can't create object of size %lu, probably out of memory.", totalSize);
        return NULL;
    }

    if (!melM_gcIsMajorCollecting(&vm->gc))
    {
        melAddNewbornItemGC(&vm->gc, newObj);
    }
    else
    {
        melM_setAgeGCItem(&vm->gc, newObj, MELON_GC_OLD_AGE);
        melAddBlackGC(&vm->gc, newObj);
    }

    vm->gc.usedBytes += objectSize;
    vm->gc.allocatedCount++;

    return newObj;
}

TRet melFreeGCItem(VM* vm, GCItem* item)
{
    vm->gc.usedBytes -= sizeof(GCItem);

    if (melM_isOldGCItem(&vm->gc, item))
    {
        melRemoveWhiteGC(&vm->gc, item);
    }

    assert(item != NULL);

#ifdef _ZERO_MEMORY_ON_FREE_GC
    memset(item, 0, item->size);
#endif

    free(item);

    vm->gc.allocatedCount--;

    return 0;
}

static TRet melVisitObject(VM* vm, GCItem* root, GCItemVisitor visit, void* ctx, TSize depth)
{
    Object* objObj = melM_objectFromObj(root);

    if (objObj->prototype != NULL)
    {
        melVisitGCItem(vm, objObj->prototype, visit, ctx, depth);
    }

    for(TSize i = 0; i < objObj->capacity; i++)
    {
        struct ObjectNode* curNode = objObj->table[i];

        while (curNode != NULL)
        {
            melVisitValue(vm, &curNode->key, visit, ctx, depth);
            melVisitValue(vm, &curNode->value, visit, ctx, depth);
            curNode = curNode->next;
        }
    }

    return 0;
}

static TRet melVisitNativeIterator(VM* vm, GCItem* root, GCItemVisitor visit, void* ctx, TSize depth)
{
    NativeIterator* nitObj = melM_nativeitFromObj(root);

    melVisitValue(vm, &nitObj->value, visit, ctx, depth);

    if (nitObj->result != NULL)
    {
        melVisitGCItem(vm, nitObj->result, visit, ctx, depth);
    }

    return 0;
}

static TRet melVisitArray(VM* vm, GCItem* root, GCItemVisitor visit, void* ctx, TSize depth)
{
    Array* arrObj = melM_arrayFromObj(root);

    for(TSize i = 0; i < arrObj->count; i++)
    {
        Value* value = melGetIndexArray(vm, root, i);
        melVisitValue(vm, value, visit, ctx, depth);
    }

    return 0;
}

static TRet melVisitFunction(VM* vm, GCItem* root, GCItemVisitor visit, void* ctx, TSize depth)
{
    assert(root->type == MELON_TYPE_FUNCTION);

    Function* f = melM_functionFromObj(root);
    StaticConstants* sc = &f->constants;
    TSize i = 0;
    
    if (f->name != NULL)
    {
        melVisitGCItem(vm, f->name, visit, ctx, depth);
    }

    for (i = 0; i < sc->count; i++)
    {
        melVisitValue(vm, &sc->constants[i], visit, ctx, depth);
    }

    FunctionPrototypes* fp = &f->funcPrototype;

    for (i = 0; i < fp->count; i++)
    {
        melVisitGCItem(vm, fp->prototypes[i], visit, ctx, depth);
    }

    return 0;
}

static TRet melVisitProgram(VM* vm, GCItem* root, GCItemVisitor visit, void* ctx, TSize depth)
{
    Program* p = melM_programFromObj(root);
    melVisitGCItem(vm, p->main, visit, ctx, depth);

    return 0;
}

static TRet melVisitSymbol(VM* vm, GCItem* root, GCItemVisitor visit, void* ctx, TSize depth)
{
    Symbol* s = melM_symbolFromObj(root);

    if (s->label != NULL)
    {
        melVisitGCItem(vm, s->label, visit, ctx, depth);
    }

    return 0;
}

static TRet melVisitClosure(VM* vm, GCItem* root, GCItemVisitor visit, void* ctx, TSize depth)
{
    Closure* cl = melM_closureFromObj(root);

    if (cl->fn != NULL)
    {
        melVisitGCItem(vm, cl->fn, visit, ctx, depth);
        
        Upvalue** upvalues = melM_closureUpvaluesFromObj(root);
        Function* fn = melM_functionFromObj(cl->fn);

        for (TSize i = 0; i < fn->upvaluesInfos.count; i++)
        {
            if (upvalues[i]->value == &upvalues[i]->closed)
            {
                melVisitValue(vm, upvalues[i]->value, visit, ctx, depth);
            }
        }
    }

    return 0;
}

TRet melVisitGCItem(VM* vm, GCItem* root, GCItemVisitor visit, void* ctx, TSize depth)
{
    assert(visit != NULL);
    
    if (visit(vm, ctx, root, depth) != 0)
    {
        return 0;
    }

    switch(root->type)
    {
        case MELON_TYPE_OBJECT:
            return melVisitObject(vm, root, visit, ctx, depth + 1);

        case MELON_TYPE_ARRAY:
            return melVisitArray(vm, root, visit, ctx, depth + 1);

        case MELON_TYPE_PROGRAM:
            return melVisitProgram(vm, root, visit, ctx, depth + 1);

        case MELON_TYPE_SYMBOL:
            return melVisitSymbol(vm, root, visit, ctx, depth + 1);

        case MELON_TYPE_FUNCTION:
            return melVisitFunction(vm, root, visit, ctx, depth + 1);

        case MELON_TYPE_CLOSURE:
            return melVisitClosure(vm, root, visit, ctx, depth + 1);

        case MELON_TYPE_NATIVEIT:
            return melVisitNativeIterator(vm, root, visit, ctx, depth + 1);

        case MELON_TYPE_RANGE:
        case MELON_TYPE_STRING:
            return 0;

        default:
            assert(0);
    }

    return 0;
}