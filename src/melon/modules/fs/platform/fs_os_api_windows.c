#include "melon/modules/fs/fs_os_api.h"
#include "melon/core/utils.h"
#include "melon/core/tstring.h"
#include "melon/core/vm.h"

#include <windows.h>

static TRet statFile(const Value* path)
{
    const char* pathData = melM_strDataFromObj(path->pack.obj);
    const char* convertedPath = melConvertToNativePath(pathData, melM_strFromObj(path->pack.obj)->len, NULL);

    if (GetFileAttributes(convertedPath) == INVALID_FILE_ATTRIBUTES)
    {
        return 1;
    }

    return 0;
}

static DWORD getFileAttributes(const Value* path)
{
    const char* pathData = melM_strDataFromObj(path->pack.obj);
    const char* convertedPath = melConvertToNativePath(pathData, melM_strFromObj(path->pack.obj)->len, NULL);

    return GetFileAttributes(convertedPath);
}

static TBool isFileType(const Value* path, TUint32 type)
{
    DWORD dwAttrib = getFileAttributes(path);

    if (dwAttrib == INVALID_FILE_ATTRIBUTES)
    {
        return 0;
    }

    return (dwAttrib & type) == type;
}

TRet melFsAPIPathExists(const Value* path, Value* result)
{
    result->type = MELON_TYPE_BOOL;
    result->pack.value.boolean = statFile(path) == 0;

    return 0;
}

TRet melFsAPIPathIsFile(const Value* path, Value* result)
{
    DWORD attr = getFileAttributes(path);
    result->type = MELON_TYPE_BOOL;
    result->pack.value.boolean = (attr != INVALID_FILE_ATTRIBUTES && attr != FILE_ATTRIBUTE_DIRECTORY);

    return 0;
}

TRet melFsAPIPathIsDirectory(const Value* path, Value* result)
{
    result->type = MELON_TYPE_BOOL;
    result->pack.value.boolean = isFileType(path, FILE_ATTRIBUTE_DIRECTORY);

    return 0;
}

TRet melFsAPIPathIsReadable(const Value* path, Value* result)
{
    return melFsAPIPathExists(path, result);
}

TRet melFsAPIPathIsWritable(const Value* path, Value* result)
{
    DWORD attr = getFileAttributes(path);
    result->type = MELON_TYPE_BOOL;
    result->pack.value.boolean = (attr != INVALID_FILE_ATTRIBUTES && attr != FILE_ATTRIBUTE_READONLY);

    return 0;
}