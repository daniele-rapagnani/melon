#include "melon/modules/path/path_os_api.h"
#include "melon/core/tstring.h"
#include "melon/core/utils.h"

// Using POSIX-2008 for realpath(arg, NULL)
#define _POSIX_C_SOURCE 200809L

#include <libgen.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static TRet transformPath(VM* vm, const Value* path, Value* result, char*(*transformFunc)(char *))
{
    assert(path);
    assert(path->type == MELON_TYPE_STRING);

    static char pathBuff[MELON_COMP_MAX_PATH_SIZE];

    char* pathData = melM_strDataFromObj(path->pack.obj);

    TSize newPathSize = 0;
    const char* convertedPath = melConvertToNativePath(pathData, melM_strFromObj(path->pack.obj)->len, &newPathSize);

    memset(&pathBuff, 0, MELON_COMP_MAX_PATH_SIZE);
    memcpy(&pathBuff, convertedPath, newPathSize);
    
    const char* pathTransformed = transformFunc(pathBuff);

    if (pathTransformed == NULL)
    {
        return 1;
    }

    TSize newLen = 0;
    const char* newPath = melConvertFromNativePath(pathTransformed, strlen(pathTransformed), &newLen);

    result->type = MELON_TYPE_STRING;
    result->pack.obj = melNewString(vm, newPath, newLen);

    return 0;
}

TRet melPathAPIDirname(VM* vm, const Value* path, Value* result)
{
    return transformPath(vm, path, result, dirname);
}

TRet melPathAPIBasename(VM* vm, const Value* path, Value* result)
{
    return transformPath(vm, path, result, basename);
}

TRet melPathAPIRealpath(VM* vm, const Value* path, Value* result)
{
    assert(path);
    assert(path->type == MELON_TYPE_STRING);

    char* pathData = melM_strDataFromObj(path->pack.obj);
    const char* convertedPath = melConvertToNativePath(pathData, melM_strFromObj(path->pack.obj)->len, NULL);
    
    char* real = realpath(convertedPath, NULL); // Using POSIX.1-2008

    if (real == NULL)
    {
        result->type = MELON_TYPE_NULL;
        return 0;
    }

    TSize newLen = 0;
    const char* newPath = melConvertFromNativePath(real, strlen(real), &newLen);

    result->type = MELON_TYPE_STRING;
    result->pack.obj = melNewString(vm, newPath, newLen);

    free(real);

    return 0;
}