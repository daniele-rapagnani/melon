#include "melon/modules/fs/fs_os_api.h"
#include "melon/core/tstring.h"
#include "melon/core/vm.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static TRet statFile(const Value* path, struct stat* s)
{
    const char* pathData = melM_strDataFromObj(path->pack.obj);

    if (stat(pathData, s) != 0)
    {
        return 1;
    }

    return 0;
}

static TBool isFileType(const Value* path, mode_t type)
{
    const char* pathData = melM_strDataFromObj(path->pack.obj);
    struct stat s;

    if (stat(pathData, &s) != 0)
    {
        return 0;
    }

    return (((s.st_mode) & S_IFMT) == type);
}

TRet melFsAPIPathExists(const Value* path, Value* result)
{
    struct stat s;
    result->type = MELON_TYPE_BOOL;
    result->pack.value.boolean = statFile(path, &s) == 0;

    return 0;
}

TRet melFsAPIPathIsFile(const Value* path, Value* result)
{
    result->type = MELON_TYPE_BOOL;
    result->pack.value.boolean = isFileType(path, S_IFREG);

    return 0;
}

TRet melFsAPIPathIsDirectory(const Value* path, Value* result)
{
    result->type = MELON_TYPE_BOOL;
    result->pack.value.boolean = isFileType(path, S_IFDIR);

    return 0;
}

TRet melFsAPIPathIsReadable(const Value* path, Value* result)
{
    const char* pathData = melM_strDataFromObj(path->pack.obj);
    result->type = MELON_TYPE_BOOL;
    result->pack.value.boolean = access(pathData, R_OK) == 0;

    return 0;
}

TRet melFsAPIPathIsWritable(const Value* path, Value* result)
{
    const char* pathData = melM_strDataFromObj(path->pack.obj);
    result->type = MELON_TYPE_BOOL;
    result->pack.value.boolean = access(pathData, W_OK) == 0;

    return 0;
}