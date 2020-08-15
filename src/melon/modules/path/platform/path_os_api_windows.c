#include "melon/modules/path/path_os_api.h"
#include "melon/core/tstring.h"

#include <windows.h>
#include <stdio.h>

static TRet transformPath(VM* vm, const Value* path, Value* result, const char*(*transformFunc)(const char *))
{
    assert(path);
    assert(path->type == MELON_TYPE_STRING);

    char* pathData = melM_strDataFromObj(path->pack.obj);
    
    const char* pathTransformed = transformFunc(pathData);

    if (pathTransformed == NULL)
    {
        result->type = MELON_TYPE_NULL;
        return 1;
    }

    result->type = MELON_TYPE_STRING;
    result->pack.obj = melNewString(vm, pathTransformed, strlen(pathTransformed));

    return 0;
}

static char driveBuf[_MAX_DRIVE + 1];
static char dirBuf[_MAX_DIR + 1];
static char fnameBuf[_MAX_FNAME + 1];
static char extBuf[_MAX_EXT + 1];

#define TOTAL_BUF_SIZE (_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT + 1)
static char totalBuf[_MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT + 1];

static TRet splitPath(const char* pathData)
{
    memset(driveBuf, 0, _MAX_DRIVE + 1);
    memset(dirBuf, 0, _MAX_DIR + 1);
    memset(fnameBuf, 0, _MAX_FNAME + 1);
    memset(extBuf, 0, _MAX_EXT + 1);
    memset(totalBuf, 0, TOTAL_BUF_SIZE);

    return _splitpath_s(
        pathData,
        driveBuf,
        _MAX_DRIVE + 1,
        dirBuf,
        _MAX_DIR + 1,
        fnameBuf,
        _MAX_FNAME + 1,
        extBuf,
        _MAX_EXT + 1
    );
}

static void normalizeResult(const char* path, char* result)
{
    if (strlen(result) == 0)
    {
        if (strncmp(path, "/", 1) == 0)
        {
            result[0] = '/';
        }
        else
        {
            result[0] = '.';
        }

        result[1] = '\0';
    }
}

static const char* transformDirname(const char* path)
{
    if (splitPath(path) != 0)
    {
        return NULL;
    }

    if (_makepath_s(totalBuf, TOTAL_BUF_SIZE, driveBuf, dirBuf, NULL, NULL) != 0)
    {
        return NULL;
    }

    normalizeResult(path, totalBuf);

    size_t totalBufLen = strlen(totalBuf);

    if (totalBufLen > 1 && totalBuf[totalBufLen - 1] == '/')
    {
        totalBuf[totalBufLen - 1] = '\0';
    }

    return totalBuf;
}

static const char* transformBasename(const char* path)
{
    if (splitPath(path) != 0)
    {
        return NULL;
    }

    if (_makepath_s(totalBuf, TOTAL_BUF_SIZE, NULL, NULL, fnameBuf, extBuf) != 0)
    {
        return NULL;
    }

    normalizeResult(path, totalBuf);
    
    return totalBuf;
}

TRet melPathAPIDirname(VM* vm, const Value* path, Value* result)
{
    return transformPath(vm, path, result, transformDirname);
}

TRet melPathAPIBasename(VM* vm, const Value* path, Value* result)
{
    return transformPath(vm, path, result, transformBasename);
}

TRet melPathAPIRealpath(VM* vm, const Value* path, Value* result)
{
    assert(path);
    assert(path->type == MELON_TYPE_STRING);

    char* pathData = melM_strDataFromObj(path->pack.obj);

    static char buffer[MAX_PATH + 1];
    memset(buffer, 0, MAX_PATH + 1);
    
    if (GetFullPathNameA(pathData, melM_strFromObj(path->pack.obj)->len, buffer, NULL) == 0)
    {
        result->type = MELON_TYPE_NULL;
        return 0;
    }

    result->type = MELON_TYPE_STRING;
    result->pack.obj = melNewString(vm, buffer, strlen(buffer));

    return 0;
}