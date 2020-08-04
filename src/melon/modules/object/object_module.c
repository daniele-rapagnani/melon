#include "melon/modules/object/object_module.h"

#include "melon/modules/modules.h"
#include "melon/core/closure.h"
#include "melon/core/symbol.h"
#include "melon/core/object.h"
#include "melon/core/tstring.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static TByte hasKeyFunc(VM* vm)
{
    melM_arg(vm, obj, MELON_TYPE_OBJECT, 0);
    melM_arg(vm, key, MELON_TYPE_NONE, 1);

    Value* res = melGetValueObject(vm, obj->pack.obj, key);

    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* result = melM_stackAllocRaw(&vm->stack);
    result->type = MELON_TYPE_BOOL;
    result->pack.value.boolean = res != NULL;

    return 1;
}

static TByte removeKeyFunc(VM* vm)
{
    melM_arg(vm, obj, MELON_TYPE_OBJECT, 0);
    melM_arg(vm, key, MELON_TYPE_NONE, 1);

    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* result = melM_stackAllocRaw(&vm->stack);
    result->type = MELON_TYPE_BOOL;
    result->pack.value.boolean = melRemoveKeyObject(vm, obj->pack.obj, key) == 0;

    return 1;
}

static TByte cloneFunc(VM* vm)
{
    melM_arg(vm, obj, MELON_TYPE_OBJECT, 0);
    melM_argOptional(vm, deep, MELON_TYPE_BOOL, 1);

    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* result = melM_stackAllocRaw(&vm->stack);
    result->type = MELON_TYPE_OBJECT;
    result->pack.obj = melCloneObject(
        vm, 
        obj->pack.obj, 
        deep->type != MELON_TYPE_NULL ? deep->pack.value.boolean : 0
    );

    return 1;
}

static TByte mergeFunc(VM* vm)
{
    melM_arg(vm, target, MELON_TYPE_OBJECT, 0);
    melM_arg(vm, with, MELON_TYPE_OBJECT, 1);
    melM_argOptional(vm, deep, MELON_TYPE_BOOL, 2);

    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* result = melM_stackAllocRaw(&vm->stack);
    result->type = MELON_TYPE_BOOL;
    result->pack.value.boolean = melMergeObject(
        vm, 
        target->pack.obj,
        with->pack.obj, 
        deep->type != MELON_TYPE_NULL ? deep->pack.value.boolean : 0
    ) == 0;

    return 1;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "hasKey", 2, 0, &hasKeyFunc},
    { "removeKey", 2, 0, &removeKeyFunc},
    { "clone", 2, 0, &cloneFunc },
    { "merge", 3, 0, &mergeFunc },
    { NULL, 0, 0, NULL }
};

static void addSymbol(VM* vm, GCItem* table, const char* name, MelonObjectSymbols op)
{
    melAddValueToObjModule(
        vm,
        table,
        melNewString(vm, name, strlen(name)),
        &MELON_SYMBOLIC_KEYS[op]
    );
}

TRet melObjectModuleInit(VM* vm)
{
    TRet val = melNewModule(vm, funcs);

    if (val != 0)
    {
        return val;
    }

    Value* module = melM_stackOffset(&vm->stack, 0);
    assert(module->type == MELON_TYPE_OBJECT);

    Value symbolsObj;
    symbolsObj.type = MELON_TYPE_OBJECT;
    symbolsObj.pack.obj = melNewObject(vm);

    // Prevent GCing when the string is created
    melM_stackPush(&vm->stack, &symbolsObj);

    melAddValueToObjModule(
        vm, 
        module->pack.obj, 
        melNewString(vm, "symbols", strlen("symbols")), 
        &symbolsObj
    );

    melM_stackPop(&vm->stack);

    addSymbol(vm, symbolsObj.pack.obj, "sumOperator", MELON_OBJSYM_ADD);
    addSymbol(vm, symbolsObj.pack.obj, "subOperator", MELON_OBJSYM_SUB);
    addSymbol(vm, symbolsObj.pack.obj, "mulOperator", MELON_OBJSYM_MUL);
    addSymbol(vm, symbolsObj.pack.obj, "divOperator", MELON_OBJSYM_DIV);
    addSymbol(vm, symbolsObj.pack.obj, "concatOperator", MELON_OBJSYM_CONCAT);
    addSymbol(vm, symbolsObj.pack.obj, "compareOperator", MELON_OBJSYM_CMP);
    addSymbol(vm, symbolsObj.pack.obj, "getIndexOperator", MELON_OBJSYM_INDEX);
    addSymbol(vm, symbolsObj.pack.obj, "setIndexOperator", MELON_OBJSYM_SETINDEX);
    addSymbol(vm, symbolsObj.pack.obj, "negOperator", MELON_OBJSYM_NEG);
    addSymbol(vm, symbolsObj.pack.obj, "sizeOperator", MELON_OBJSYM_SIZEARR);
    addSymbol(vm, symbolsObj.pack.obj, "powOperator", MELON_OBJSYM_POW);
    addSymbol(vm, symbolsObj.pack.obj, "callOperator", MELON_OBJSYM_CALL);
    addSymbol(vm, symbolsObj.pack.obj, "hashingFunction", MELON_OBJSYM_HASH);
    addSymbol(vm, symbolsObj.pack.obj, "iterator", MELON_OBJSYM_ITERATOR);
    addSymbol(vm, symbolsObj.pack.obj, "nextFunction", MELON_OBJSYM_NEXT);
    addSymbol(vm, symbolsObj.pack.obj, "getPropertyOperator", MELON_OBJSYM_PROPERTY);
    addSymbol(vm, symbolsObj.pack.obj, "setPropertyOperator", MELON_OBJSYM_SETPROPERTY);

    return 0;
}