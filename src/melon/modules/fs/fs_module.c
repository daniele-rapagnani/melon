#include "melon/modules/fs/fs_module.h"
#include "melon/modules/fs/fs_os_api.h"
#include "melon/modules/modules.h"

#include <stdlib.h>
#include <assert.h>

#define wrappedName(fn) wrapper__##fn

#define wrapBoolFunction(fn)                      \
    TByte wrappedName(fn)(VM *vm)                 \
    {                                             \
        melM_arg(vm, path, MELON_TYPE_STRING, 0); \
        Value result;                             \
        result.type = MELON_TYPE_NULL;            \
                                                  \
        fn(path, &result);                        \
        melM_stackPush(&vm->stack, &result);        \
                                                  \
        return 1;                                 \
                                                  \
    }

wrapBoolFunction(melFsAPIPathExists);
wrapBoolFunction(melFsAPIPathIsFile);
wrapBoolFunction(melFsAPIPathIsDirectory);
wrapBoolFunction(melFsAPIPathIsReadable);
wrapBoolFunction(melFsAPIPathIsWritable);

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "exists", 1, 0, wrappedName(melFsAPIPathExists) },
    { "isFile", 1, 0, wrappedName(melFsAPIPathIsFile) },
    { "isDirectory", 1, 0, wrappedName(melFsAPIPathIsDirectory) },
    { "isReadable", 1, 0, wrappedName(melFsAPIPathIsReadable) },
    { "isWritable", 1, 0, wrappedName(melFsAPIPathIsWritable) },
    { NULL, 0, 0, NULL }
};

TRet melFsModuleInit(VM* vm)
{
    return melNewModule(vm, funcs);
}