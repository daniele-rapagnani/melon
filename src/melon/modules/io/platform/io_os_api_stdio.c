#include "melon/modules/io/io_os_api.h"
#include "melon/core/utils.h"
#include "melon/core/tstring.h"

#include <stdio.h>
#include <stdlib.h>

TRet melStdioAPIOpenFile(Value* val, const char* name, const char* flags)
{
    const char* convertedPath = melConvertToNativePath(name, strlen(name), NULL);
    FILE* f = fopen(name, flags);

    if (f == NULL)
    {
        val->type = MELON_TYPE_NULL;
        return 1;
    }

    val->type = MELON_TYPE_NATIVEPTR;
    val->pack.obj = (GCItem*)f;

    return 0;
}

TRet melStdioAPICloseFile(const Value* fd)
{
    if (fd->type != MELON_TYPE_NATIVEPTR)
    {
        return 1;
    }

    return fclose((FILE*)fd->pack.obj) == 0 ? 0 : 1;
}

TRet melStdioAPIReadFile(VM* vm, const Value* fd, TSize count, Value* result)
{
    result->type = MELON_TYPE_NULL;

    if (fd->type != MELON_TYPE_NATIVEPTR)
    {
        return 1;
    }

    FILE* f = (FILE*)fd->pack.obj;

    if (f == stdin && count == 0)
    {
        melM_fatal(vm, "A read count must be specified when reading from stdin");
        return 1;
    }

    TSize dataSize = count;

    if (count == 0)
    {
        long curSeek = ftell(f);

        if (fseek(f, 0, SEEK_END) != 0)
        {
            return 1;
        }

        TSize size = ftell(f);
        dataSize = curSeek > size ? 0 : size - curSeek;

        if (fseek(f, curSeek, SEEK_SET) != 0)
        {
            return 1;
        }
    }

    GCItem* strData = melNewDataString(vm, dataSize + 1);
    char* data = melM_strDataFromObj(strData);

    if (fread(data, 1, dataSize, f) == 0)
    {
        return 1;
    }

    // Null terminate the string so that the string
    // can be read as a null-terminated string if it's needed.
    data[dataSize] = '\0';

    if (count == 0)
    {
        // fseek resets the eof flag so if
        // we read the whole file let's set the eof flag
        // by reading the next char

        fgetc(f);
    }

    if (melUpdateStringHash(vm, strData) != 0)
    {
        return 1;
    }

    result->type = strData->type;
    result->pack.obj = strData;

    return 0;
}

TRet melStdioAPIWriteFile(VM* vm, const Value* fd, const Value* data)
{
    if (fd->type != MELON_TYPE_NATIVEPTR)
    {
        return 1;
    }

    if (data->type != MELON_TYPE_STRING)
    {
        return 1;
    }

    String* dataStr = melM_strFromObj(data->pack.obj);
    const char* dataData = melM_strDataFromObj(data->pack.obj);

    if (fwrite(dataData, 1, dataStr->len, (FILE*)fd->pack.obj) != dataStr->len)
    {
        return 1;
    }

    return 0;
}

TRet melStdioAPIFlushFile(VM* vm, const Value* fd)
{
    if (fd->type != MELON_TYPE_NATIVEPTR)
    {
        return 1;
    }

    return fflush((FILE*)fd->pack.obj) == 0 ? 0 : 1;
}

TRet melStdioAPITellFile(VM* vm, const Value* fd, Value* result)
{
    if (fd->type != MELON_TYPE_NATIVEPTR)
    {
        return 1;
    }

    result->type = MELON_TYPE_INTEGER;
    result->pack.value.integer = ftell((FILE*)fd->pack.obj);

    return 0;
}

TRet melStdioAPISeekFile(VM* vm, const Value* fd, const Value* offset, TBool fromEnd)
{
    if (fd->type != MELON_TYPE_NATIVEPTR)
    {
        return 1;
    }

    if (offset->type != MELON_TYPE_INTEGER)
    {
        return 1;
    }

    if (fseek((FILE*)fd->pack.obj, offset->pack.value.integer, fromEnd ? SEEK_END : SEEK_SET) != 0)
    {
        return 1;
    }

    return 0;
}

TRet melStdioAPISizeFile(VM* vm, const Value* fd, Value* result)
{
    result->type = MELON_TYPE_NULL;

    if (fd->type != MELON_TYPE_NATIVEPTR)
    {
        return 1;
    }

    FILE* f = (FILE*)fd->pack.obj;

    TSize curPos = ftell(f);

    if (fseek(f, 0, SEEK_END) != 0)
    {
        return 1;
    }

    TSize size = ftell(f);

    if (fseek(f, curPos, SEEK_SET) != 0)
    { 
        return 1;
    }

    result->type = MELON_TYPE_INTEGER;
    result->pack.value.integer = size;

    return 0;
}

TRet melStdioAPIIsEOFFile(VM* vm, const Value* fd, Value* result)
{
    result->type = MELON_TYPE_NULL;

    if (fd->type != MELON_TYPE_NATIVEPTR)
    {
        return 1;
    }

    FILE* f = (FILE*)fd->pack.obj;

    result->type = MELON_TYPE_BOOL;
    result->pack.value.boolean = feof(f);

    return 0;
}

TRet melStdioAPIGetStdin(VM*vm , Value* result)
{
    result->type = MELON_TYPE_NATIVEPTR;
    result->pack.obj = (GCItem*)stdin;

    return 0;
}

TRet melStdioAPIGetStderr(VM*vm , Value* result)
{
    result->type = MELON_TYPE_NATIVEPTR;
    result->pack.obj = (GCItem*)stderr;

    return 0;
}

TRet melStdioAPIGetStdout(VM*vm , Value* result)
{
    result->type = MELON_TYPE_NATIVEPTR;
    result->pack.obj = (GCItem*)stdout;

    return 0;
}
