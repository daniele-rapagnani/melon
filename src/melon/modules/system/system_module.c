#include "melon/modules/system/system_module.h"

#include "melon/modules/modules.h"
#include "melon/core/tstring.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

static TByte getMelonVersion(VM* vm)
{
    GCItem* ver = melNewString(vm, MELON_VERSION, strlen(MELON_VERSION));
    melM_vstackPushGCItem(&vm->stack, ver);

    return 1;
}

static TByte getArchBits(VM* vm)
{
    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* result = melM_stackAllocRaw(&vm->stack);
    result->type = MELON_TYPE_INTEGER;

#ifdef MELON_64BIT
    result->pack.value.integer = 64;
#else
    result->pack.value.integer = 32;
#endif

    return 1;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "getArchBits", 0, 0, &getArchBits },
    { "getMelonVersion", 0, 0, &getMelonVersion },
    { NULL, 0, 0, NULL }
};

TRet melSystemModuleInit(VM* vm)
{
    return melNewModule(vm, funcs);
}