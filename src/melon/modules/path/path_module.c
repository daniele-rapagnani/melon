#include "melon/modules/path/path_module.h"
#include "melon/modules/path/path_os_api.h"
#include "melon/core/utils.h"
#include "melon/modules/modules.h"

#include <stdlib.h>

static TByte wrapTransformFunc(VM* vm, TRet(*transformFunc)(VM*, const Value*, Value*))
{
    melM_arg(vm, path, MELON_TYPE_STRING, 0);
    Value result;
    
    if (transformFunc(vm, path, &result) == 1)
    {
        melM_vstackPushNull(&vm->stack);
        return 1;
    }

    melM_stackPush(&vm->stack, &result);
    return 1;
}

TByte dirnameFunc(VM* vm)
{
   return wrapTransformFunc(vm, melPathAPIDirname);
}

TByte basenameFunc(VM* vm)
{
    return wrapTransformFunc(vm, melPathAPIBasename);
}

TByte realpathFunc(VM* vm)
{
    return wrapTransformFunc(vm, melPathAPIRealpath);
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "dirname", 1, 0, dirnameFunc },
    { "basename", 1, 0, basenameFunc },
    { "realpath", 1, 0, realpathFunc },
    { NULL, 0, 0, NULL }
};

TRet melPathModuleInit(VM* vm)
{
    return melNewModule(vm, funcs);
}