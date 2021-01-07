#include "melon/modules/fs/fs_module.h"
#include "melon/modules/fs/fs_os_api.h"
#include "melon/modules/modules.h"

/***
 * @module
 * 
 * This module exposes functions for filesystem manipulation.
 */

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

/***
 * @function exists
 * 
 * Checks whether a given path exists on file system.
 * 
 * @arg path The path to check
 * @returns `true` if the provided path exists, `false` otherwise
 */

wrapBoolFunction(melFsAPIPathExists);

/***
 * @function isFile
 * 
 * Checks whether a given path corresponds to a valid file on filesystem.
 * 
 * @arg path The path to check
 * @returns `true` if the provided path is a file, `false` otherwise
 */

wrapBoolFunction(melFsAPIPathIsFile);

/***
 * @function isDirectory
 * 
 * Checks whether a given path corresponds to a directory on filesystem.
 * 
 * @arg path The path to check
 * @returns `true` if the provided path is a directory, `false` otherwise
 */

wrapBoolFunction(melFsAPIPathIsDirectory);

/***
 * @function isReadable
 * 
 * Checks whether a given path is readable.
 * 
 * @arg path The path to check
 * @returns `true` if the provided path is readable, `false` otherwise
 */

wrapBoolFunction(melFsAPIPathIsReadable);

/***
 * @function isWritable
 * 
 * Checks whether a given path is writable.
 * 
 * @arg path The path to check
 * @returns `true` if the provided path is writable, `false` otherwise
 */

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