#include "melon/modules/path/path_os_api.h"
#include "melon/core/tstring.h"

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
    memset(&pathBuff, 0, MELON_COMP_MAX_PATH_SIZE);
    memcpy(&pathBuff, pathData, melM_strFromObj(path->pack.obj)->len);
    
    const char* pathTransformed = transformFunc(pathBuff);

    if (pathTransformed == NULL)
    {
        return 1;
    }

    result->type = MELON_TYPE_STRING;
    result->pack.obj = melNewString(vm, pathTransformed, strlen(pathTransformed));

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
    char* real = realpath(pathData, NULL); // Using POSIX.1-2008

    if (real == NULL)
    {
        result->type = MELON_TYPE_NULL;
        return 0;
    }

    result->type = MELON_TYPE_STRING;
    result->pack.obj = melNewString(vm, real, strlen(real));

    free(real);

    return 0;
}