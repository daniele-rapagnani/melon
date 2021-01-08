#include "melon/modules/path/path_module.h"
#include "melon/modules/path/path_os_api.h"
#include "melon/core/utils.h"
#include "melon/modules/modules.h"

/***
 * @module
 * 
 * This module can be used to manipulate `String` values that represent
 * filesystem paths.
 */

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

/***
 * Returns the last directory component of the provided path
 * 
 * @arg path The path from which the dirname will be extracted
 * @returns The extracted directory component or `null` on error
 */

static TByte dirnameFunc(VM* vm)
{
   return wrapTransformFunc(vm, melPathAPIDirname);
}

/***
 * Returns the file component of the provided path
 * 
 * @arg path The path from which the basename will be extracted
 * @returns The extracted file component or `null` on error
 */

static TByte basenameFunc(VM* vm)
{
    return wrapTransformFunc(vm, melPathAPIBasename);
}

/***
 * Resolves any relative components or symbolic links of a given path
 * and returns the real absolute path.
 * 
 * @arg path The path to be transformed to a real absolute path
 * @returns The real path or `null` on error
 */

static TByte realpathFunc(VM* vm)
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