#include "melon/modules/gc/gc_module.h"
#include "melon/modules/modules.h"
#include "melon/core/gc.h"

#include <stdlib.h>
#include <assert.h>

static TByte triggerFunc(VM* vm)
{
    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* result = melM_stackAllocRaw(&vm->stack);
    result->type = MELON_TYPE_BOOL;
    result->pack.value.boolean = melTriggerGC(vm, &vm->gc) == 0;

    return 1;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "trigger", 0, 0, &triggerFunc },
    { NULL, 0, 0, NULL }
};

TRet melGcModuleInit(VM* vm)
{
    return melNewModule(vm, funcs);
}