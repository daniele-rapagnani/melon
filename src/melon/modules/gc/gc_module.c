#include "melon/modules/gc/gc_module.h"
#include "melon/modules/modules.h"
#include "melon/core/gc.h"
#include "melon/core/utils.h"
#include "melon/core/tstring.h"

/***
 * @module
 * 
 * This module exposes an interface with the VM's garbage collector.
 */

#include <stdlib.h>
#include <assert.h>

/***
 * Forces the GC to be triggered right away.
 * This call will be ignored if the GC is already running incrementally
 * and `false` will be returned.
 * 
 * @returns `true` if the GC could be triggered, `false` if it couldn't.
 */

static TByte triggerFunc(VM* vm)
{
    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* result = melM_stackAllocRaw(&vm->stack);
    result->type = MELON_TYPE_BOOL;
    result->pack.value.boolean = melTriggerGC(vm, &vm->gc) == 0;

    return 1;
}

/***
 * Returns a formatted string with some information on the
 * GC status of a GC managed value.
 * 
 * @arg val The value to inspect
 * @returns A string with GC related information for the provided value
 */

static TByte infoFunc(VM* vm)
{
    melM_arg(vm, obj, MELON_TYPE_NONE, 0);

    if (!melM_isGCItem(obj))
    {
        melM_vstackPushNull(&vm->stack);
        return 1;
    }

    struct StrFormat sf;
    memset(&sf, 0, sizeof(struct StrFormat));

    melStringFmtUtils(
        &sf,
        "(%p) age = %d, old = %s, color = %d, dark = %s",
        obj->pack.obj,
        melM_getAgeGCItem(&vm->gc, obj->pack.obj),
        melM_isOldGCItem(&vm->gc, obj->pack.obj) ? "yes" : "no",
        melM_getColorGCItem(&vm->gc, obj->pack.obj),
        melM_isDarkGCItem(&vm->gc, obj->pack.obj) ? "yes" : "no"
    );

    Value val;
    val.pack.obj = melNewString(vm, sf.buffer, sf.used);
    val.type = val.pack.obj->type;

    melM_stackPush(&vm->stack, &val);

    melStringFmtFreeUtils(&sf);

    return 1;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "trigger", 0, 0, &triggerFunc },
    { "info", 1, 0, &infoFunc },
    { NULL, 0, 0, NULL }
};

TRet melGcModuleInit(VM* vm)
{
    return melNewModule(vm, funcs);
}