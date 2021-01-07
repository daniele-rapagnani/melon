#include "melon/modules/object/object_module.h"

#include "melon/modules/modules.h"
#include "melon/core/closure.h"
#include "melon/core/symbol.h"
#include "melon/core/object.h"
#include "melon/core/tstring.h"

/***
 * @module
 * 
 * This module groups together utility functions that can be used to manipulate
 * [`Object`](object.md) values.
 * 
 * It also exposes [`Symbols`](symbol.md) that can be used to customise an `Object`'s
 * behavior.
 * 
 * @exports symbols.sumOperator A symbol that can be used to overload the sum operator
 * @exports symbols.subOperator A symbol that can be used to overload the subtraction operator
 * @exports symbols.mulOperator A symbol that can be used to overload the multiplication operator
 * @exports symbols.divOperator A symbol that can be used to overload the division operator
 * @exports symbols.concatOperator A symbol that can be used to overload the concatenation operator
 * @exports symbols.compareOperator A symbol that can be used to provide custom comparison
 * @exports symbols.getIndexOperator A symbol that can be used to overload the indexed access operator
 * @exports symbols.setIndexOperator A symbol that can be used to overload the indexed set operator 
 * @exports symbols.negOperator A symbol that can be used to overload the negation operator
 * @exports symbols.sizeOperator A symbol that can be used to overload the size operator
 * @exports symbols.powOperator A symbol that can be used to overload the power operator
 * @exports symbols.callOperator A symbol that can be used to overload the call operator
 * @exports symbols.hashingFunction A symbol that can be used provide custom hashing mechanism
 * @exports symbols.iterator A symbol that can be used to overload the get iterator operator
 * @exports symbols.nextFunction A symbol that can be used to overload the next iterator's value operator
 * @exports symbols.getPropertyOperator A symbol that can be used to overload the dot operator for property reading
 * @exports symbols.setPropertyOperator A symbol that can be used to overload the dot operator for property writing
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/***
 * Checks if the provided object has the requested key defined.
 * This can be used to be sure that an object has a key even if the associated value is `null`.
 * 
 * @arg obj The object holding the key
 * @arg key The key to check
 * 
 * @returns `true` if the key is present, `false` otherwise.
 */

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

/***
 * Removes a given key from an `Object`.
 * This may cause some internal re-allocation to be triggered and it's not the
 * same as setting a key to `null` as the key will not appear in enumeration.
 * 
 * @arg obj The object holding the key
 * @arg key The key to remove
 * 
 * @returns `true` if the key could be found, `false` otherwise.
 */

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

/***
 * Clones an `Object` returning a copy of the original.
 * When a deep clone is performed only objects will be cloned,
 * values which are not objects but are managed by the GC (eg: arrays)
 * will still point to the same value and will be reference assigned.
 * 
 * @arg obj The object to be cloned
 * @arg ?deep `true` if it should be cloned recursively
 * 
 * @returns The cloned object.
 */

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

/***
 * Merges two objects adding to `target` any key that is missing from it
 * but that was found in `with`.
 * 
 * @arg target The object that will be modified
 * @arg with The object with the keys to add to `target` is any is missing
 * @arg ?deep `true` if the merging should be done recursively. Defaults to `false`
 * 
 * @returns `true` if the two objects were merged successfuly, `false` otherwise.
 */

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