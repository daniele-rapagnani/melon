#include "melon/modules/types/types_module.h"

#include "melon/modules/modules.h"
#include "melon/core/closure.h"
#include "melon/core/tstring.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define melM_defineCheckType(typeName) \
    static TByte is##typeName(VM* vm) \
    { \
        Value *input = melM_stackOffset(&vm->stack, 0);              \
        Value res; \
        res.type = MELON_TYPE_BOOL; \
        \
        res.pack.value.boolean = \
            (input != NULL && input->type == MELON_TYPE_##typeName) ? 1 : 0 \
        ; \
        \
        melM_stackPush(&vm->stack, &res); \
        return 1; \
    }

#define melM_defineModuleFunc(funcName, typeName) \
    { #funcName, 1, 0, is##typeName },

melM_defineCheckType(INTEGER)
melM_defineCheckType(NUMBER)
melM_defineCheckType(BOOL)
melM_defineCheckType(NULL)
melM_defineCheckType(STRING)
melM_defineCheckType(OBJECT)
melM_defineCheckType(CLOSURE)
melM_defineCheckType(PROGRAM)
melM_defineCheckType(ARRAY)

TByte ofFunc(VM* vm)
{
    melM_arg(vm, val, MELON_TYPE_NONE, 0);

    melM_vstackPushGCItem(
        &vm->stack, 
        melNewString(vm, MELON_TYPES_NAMES[val->type], strlen(MELON_TYPES_NAMES[val->type]))
    );

    return 1;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    melM_defineModuleFunc(isInteger, INTEGER)
    melM_defineModuleFunc(isNumber, NUMBER)
    melM_defineModuleFunc(isBool, BOOL)
    melM_defineModuleFunc(isNull, NULL)
    melM_defineModuleFunc(isString, STRING)
    melM_defineModuleFunc(isObject, OBJECT)
    melM_defineModuleFunc(isClosure, CLOSURE)
    melM_defineModuleFunc(isProgram, PROGRAM)
    melM_defineModuleFunc(isArray, ARRAY)
    { "of", 1, 0, ofFunc },
    { NULL, 0, 0, NULL }
};

TRet melTypesModuleInit(VM* vm)
{
    return melNewModule(vm, funcs);
}