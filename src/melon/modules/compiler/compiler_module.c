#include "melon/modules/compiler/compiler_module.h"
#include "melon/modules/modules.h"
#include "melon/core/utils.h"
#include "melon/core/compiler.h"
#include "melon/core/closure.h"
#include "melon/core/tstring.h"
#include "melon/modules/modules.h"

#include <assert.h>
#include <string.h>

static Value ioKey;
static Value openFileKey;
static Value readFileKey;

static TByte compileString(VM* vm)
{
    melM_arg(vm, arg, MELON_TYPE_STRING, 0);
    melM_argOptional(vm, cb, MELON_TYPE_CLOSURE, 1);

    if (melNewModuleFromSource(vm, NULL, arg->pack.obj, cb->type != MELON_TYPE_NULL ? cb->pack.obj : NULL) != 0)
    {
        Value nullVal;
        nullVal.type = MELON_TYPE_NULL;
        melM_stackPush(&vm->stack, &nullVal);
    }
    
    return 1;
}

static TByte compileFile(VM* vm)
{
    melM_arg(vm, arg, MELON_TYPE_STRING, 0);
    melM_argOptional(vm, cb, MELON_TYPE_CLOSURE, 1);

    Value* openFileFunc = melGetClosureFromModule(vm, &ioKey, &openFileKey);
    assert(openFileFunc != NULL);

    melM_stackPush(&vm->stack, openFileFunc);
    melM_stackPush(&vm->stack, arg);

    melCallClosureSyncVM(vm, 1, 0, 1);

    Value fileResult = *melM_stackOffset(&vm->stack, 0);

    if (fileResult.type != MELON_TYPE_OBJECT)
    {
        melM_stackPop(&vm->stack);
        melM_vstackPushNull(&vm->stack);
        return 1;
    }
    
    melSaveAndPauseGC(vm, &vm->gc);
    melM_stackPop(&vm->stack);

    Value* readFileFunc = melGetClosureFromModule(vm, &ioKey, &readFileKey);
    assert(readFileFunc != NULL);

    melM_stackPush(&vm->stack, readFileFunc);
    melM_stackPush(&vm->stack, &fileResult);
    melRestorePauseGC(vm, &vm->gc);

    melCallClosureSyncVM(vm, 1, 0, 1);

    Value contentResult = *melM_stackOffset(&vm->stack, 0);

    if (contentResult.type != MELON_TYPE_STRING)
    {
        melM_stackPop(&vm->stack);
        melM_vstackPushNull(&vm->stack);
        return 1;
    }

    melSaveAndPauseGC(vm, &vm->gc);
    melM_stackPop(&vm->stack);
    melSetLocalVM(vm, melGetTopCallFrameVM(vm), 2, &contentResult, 0);
    melRestorePauseGC(vm, &vm->gc);

    melM_arg(vm, source, MELON_TYPE_STRING, 2);

    if (melNewModuleFromSource(vm, arg->pack.obj, source->pack.obj, cb->type != MELON_TYPE_NULL ? cb->pack.obj : NULL) != 0)
    {
        Value nullVal;
        nullVal.type = MELON_TYPE_NULL;
        melM_stackPush(&vm->stack, &nullVal);
    }

    return 1;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "compileFile", 2, 1, compileFile },
    { "compile", 2, 0, compileString },
    { NULL, 0, 0, NULL }
};

TRet melCompilerModuleInit(VM* vm)
{
    if (melCreateKeyModule(vm, "io", &ioKey) != 0)
    {
        return 1;
    }

    if (melCreateKeyModule(vm, "open", &openFileKey) != 0)
    {
        return 1;
    }

    if (melCreateKeyModule(vm, "read", &readFileKey) != 0)
    {
        return 1;
    }
  
    return melNewModule(vm, funcs);
}