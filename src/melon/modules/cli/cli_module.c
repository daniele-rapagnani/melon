#include "melon/modules/cli/cli_module.h"
#include "melon/modules/modules.h"
#include "melon/core/array.h"
#include "melon/core/tstring.h"

#include <stdlib.h>
#include <assert.h>

static const char** argv = NULL;
static int argc = 0;

TRet melSetArgs(VM* vm, const char** eargv, int eargc)
{
    argv = eargv;
    argc = eargc;

    return 0;
}

static TByte getArgsFunc(VM* vm)
{
    GCItem* arr = melNewArray(vm);
    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* arrVal = melM_stackAllocRaw(&vm->stack);
    arrVal->type = arr->type;
    arrVal->pack.obj = arr;

    for (int i = 0; i < argc; i++)
    {
        Value strVal;
        strVal.type = MELON_TYPE_STRING;
        strVal.pack.obj = melNewString(vm, argv[i], strlen(argv[i]));
        melPushArray(vm, arr, &strVal);
    }

    return 1;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "getArgs", 0, 0, &getArgsFunc },
    { NULL, 0, 0, NULL }
};

TRet melCliModuleInit(VM* vm)
{
    return melNewModule(vm, funcs);
}