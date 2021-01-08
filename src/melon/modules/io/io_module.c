#include "melon/modules/io/io_module.h"
#include "melon/modules/io/io_os_api.h"
#include "melon/modules/modules.h"
#include "melon/core/utils.h"
#include "melon/core/tstring.h"
#include "melon/core/array.h"
#include "melon/core/object.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

/***
 * @module
 * This module is dedicated to input/output operations.
 * 
 * @exports stdin The standard input file
 * @exports stdout The standard output file
 * @exports stderr The standard error file
 */

static Value stringUtilsKey;
static Value toStringKey;
static Value fileDescSymbol;

static Value* ensureFD(VM* vm, Value* val)
{
    if (val->type != MELON_TYPE_OBJECT)
    {
        melErrorVM(
            vm,
            "A file object was expected but a '%s' was provided",
            MELON_TYPES_NAMES[val->type]
        );
    }

    Value* fd = melGetValueObject(vm, val->pack.obj, &fileDescSymbol);

    if (fd == NULL)
    {
        melErrorVM(
            vm,
            "An invalid file object was provided"
        );
    }

    return fd;
}

static void pushNewFileObj(VM* vm, Value* path, Value* flags)
{
    String* pathObj = melM_strFromObj(path->pack.obj);

    if (pathObj->len == 0)
    {
        melM_vstackPushNull(&vm->stack);
        return;
    }

    const char* flagsStr = "r";
    const char* pathStr = melM_strDataFromObj(path->pack.obj);

    if (flags->type == MELON_TYPE_STRING)
    {   
        String* flagsObj = melM_strFromObj(flags->pack.obj);
        flagsStr = melM_strDataFromObj(flags->pack.obj);
    }

    Value fd;

    if (melStdioAPIOpenFile(&fd, pathStr, flagsStr) != 0)
    {
        melM_vstackPushNull(&vm->stack);
        return;
    }

    GCItem* fileObj = melNewObject(vm);
    melSetValueObject(vm, fileObj, &fileDescSymbol, &fd);
    
    melM_vstackPushGCItem(&vm->stack, fileObj);
}

/***
 * Opens a file and returns an object that can be used to
 * do I/O on that file.
 * 
 * @arg path The path to the file to open
 * @arg flags The open flags, os/context dependent, such as `"w"`, `"r"`, `"w+"`, ...
 * 
 * @returns A valid descriptor or null
 */

static TByte openFileFunction(VM* vm)
{
    melM_arg(vm, path, MELON_TYPE_STRING, 0);
    melM_argOptional(vm, flags, MELON_TYPE_STRING, 1);

    pushNewFileObj(vm, path, flags);
    return 1;
}

/***
 * Closes a file that was previously open.
 * 
 * @arg file A file descriptor returned by `open`
 * 
 * @returns `true` on success, `false` otherwise
 */

static TByte closeFileFunction(VM* vm)
{
    melM_arg(vm, file, MELON_TYPE_OBJECT, 0);
    Value* fd = ensureFD(vm, file);

    TRet ret = melStdioAPICloseFile(fd);
    Value result;
    result.type = MELON_TYPE_BOOL;
    result.pack.value.boolean = ret == 0;

    melM_stackPush(&vm->stack, &result);
    
    return 1;
}

/***
 * Reads data from a file.
 * 
 * @arg file A file descriptor returned by `open`
 * @arg ?bytes The maximum number of bytes to read
 * 
 * @returns `true` on success, `false` otherwise
 */

static TByte readFileFunction(VM* vm)
{
    melM_arg(vm, file, MELON_TYPE_OBJECT, 0);
    melM_argOptional(vm, size, MELON_TYPE_INTEGER, 1);

    Value* fd = ensureFD(vm, file);

    TInteger count = 0;

    if (size->type == MELON_TYPE_INTEGER)
    {
        count = size->pack.value.integer;
    }

    Value result;
    melStdioAPIReadFile(vm, fd, count, &result);
    melM_stackPush(&vm->stack, &result);

    return 1;
}

/***
 * Writes data to a file descriptor.
 * 
 * @arg file A file descriptor returned by `open`
 * @arg data A string with the data to be written
 * 
 * @returns `true` on success, `false` otherwise
 */

static TByte writeFileFunction(VM* vm)
{
    melM_arg(vm, file, MELON_TYPE_OBJECT, 0);
    melM_arg(vm, data, MELON_TYPE_STRING, 1);

    Value* fd = ensureFD(vm, file);

    Value result;
    result.type = MELON_TYPE_BOOL;
    result.pack.value.boolean = melStdioAPIWriteFile(vm, fd, data) == 0;

    melM_stackPush(&vm->stack, &result);
    return 1;
}

/***
 * Flushes the buffer for a file, writing any pending changes immediately.
 * 
 * @arg file A file descriptor returned by `open`
 * 
 * @returns `true` on success, `false` otherwise
 */

static TByte flushFileFunction(VM* vm)
{
    melM_arg(vm, file, MELON_TYPE_OBJECT, 0);
    Value* fd = ensureFD(vm, file);

    Value result;
    result.type = MELON_TYPE_BOOL;
    result.pack.value.boolean = melStdioAPIFlushFile(vm, fd) == 0;

    melM_stackPush(&vm->stack, &result);
    return 1;
}

/***
 * Gets the current position inside a file
 * 
 * @arg file A file descriptor returned by `open`
 * 
 * @returns The number of bytes into the file
 */

static TByte tellFileFunction(VM* vm)
{
    melM_arg(vm, file, MELON_TYPE_OBJECT, 0);
    Value* fd = ensureFD(vm, file);

    Value result;
    melStdioAPITellFile(vm, fd, &result);
    melM_stackPush(&vm->stack, &result);
    return 1;
}

/***
 * Seeks the current position inside the file to a given value
 * 
 * @arg file A file descriptor returned by `open`
 * @arg offset An integer with the new offset in bytes
 * @arg ?fromEnd If `true` sets the new position counting from the end of the file
 * 
 * @returns `true` on success, `false` otherwise
 */

static TByte seekFileFunction(VM* vm)
{
    melM_arg(vm, file, MELON_TYPE_OBJECT, 0);
    melM_arg(vm, offset, MELON_TYPE_INTEGER, 1);
    melM_argOptional(vm, fromEnd, MELON_TYPE_BOOL, 2);

    Value* fd = ensureFD(vm, file);
    TBool fromEndBool = 0;

    if (fromEnd->type == MELON_TYPE_BOOL)
    {
        fromEndBool = fromEnd->pack.value.boolean;
    }

    Value result;
    result.type = MELON_TYPE_BOOL;
    result.pack.value.boolean = melStdioAPISeekFile(vm, fd, offset, fromEndBool) == 0;

    melM_stackPush(&vm->stack, &result);
    
    return 1;
}

/***
 * Returns the size of a given open file descriptor.
 * 
 * @arg file A file descriptor returned by `open`
 * 
 * @returns The number of total bytes for the file
 */

static TByte sizeFileFunction(VM* vm)
{
    melM_arg(vm, file, MELON_TYPE_OBJECT, 0);
    Value* fd = ensureFD(vm, file);

    Value result;
    melStdioAPISizeFile(vm, fd, &result);
    melM_stackPush(&vm->stack, &result);
    return 1;
}

/***
 * Checks if the end of file has been reached for a file descriptor.
 * 
 * @arg file A file descriptor returned by `open`
 * 
 * @returns `true` if the EOF has been reached, `false` otherwise
 */

static TByte isEOFFileFunction(VM* vm)
{
    melM_arg(vm, file, MELON_TYPE_OBJECT, 0);
    Value* fd = ensureFD(vm, file);

    Value result;
    melStdioAPIIsEOFFile(vm, fd, &result);
    melM_stackPush(&vm->stack, &result);
    return 1;
}

/***
 * This function prints one or more values directly to `stdout`.
 * Each value is first converted to a string and then output.
 * A space is used as a separator between each value.
 * 
 * @arg ... A list of values to print
 */

static TByte printLineFunction(VM* vm)
{
    melM_arg(vm, arg, MELON_TYPE_ARRAY, 0);

    Value* toStringFunc = melGetClosureFromModule(vm, &stringUtilsKey, &toStringKey);
    Array* argArr = melM_arrayFromObj(arg->pack.obj);

    struct StrFormat sf;
    memset(&sf, 0, sizeof(struct StrFormat));

    for (TSize i = 0; i < argArr->count; i++)
    {
        melM_stackPush(&vm->stack, toStringFunc);
        melM_stackPush(&vm->stack, melGetIndexArray(vm, arg->pack.obj, i));

        melCallClosureSyncVM(vm, 1, 0, 1);

        Value* stringResult = melM_stackOffset(&vm->stack, 0);

        if (stringResult->type != MELON_TYPE_STRING)
        {
            melM_fatal(vm, "The toString function should always return a string.");
            return 0;
        }

        melStringFmtUtils(
            &sf, 
            "%.*s%s", 
            melM_strFromObj(stringResult->pack.obj)->len, 
            melM_strDataFromObj(stringResult->pack.obj),
            i < argArr->count - 1 ? " " : ""
        );

        // Remove the string now
        melM_stackPop(&vm->stack);
    }

    melStringFmtWriteChar(&sf, '\n');
    melPrintVM(vm, &sf);
    melStringFmtFreeUtils(&sf);

    return 0;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "print", 1, 0, printLineFunction, 0, 1 },
    { "open", 2, 0, openFileFunction },
    { "close", 1, 0, closeFileFunction },
    { "read", 2, 0, readFileFunction },
    { "write", 2, 0, writeFileFunction },
    { "flush", 1, 0, flushFileFunction },
    { "tell", 1, 0, tellFileFunction },
    { "seek", 3, 0, seekFileFunction },
    { "size", 1, 0, sizeFileFunction },
    { "isEOF", 1, 0, isEOFFileFunction },
    { NULL, 0, 0, NULL }
};

TRet melIoModuleInit(VM* vm)
{
    if (melCreateGlobalSymbolKey(vm, "File descriptor", &fileDescSymbol) != 0)
    {
        return 1;
    }

    if (melCreateKeyModule(vm, "string", &stringUtilsKey) != 0)
    {
        return 1;
    }

    if (melCreateKeyModule(vm, "toString", &toStringKey) != 0)
    {
        return 1;
    }

    TRet val = melNewModule(vm, funcs);

    if (val != 0)
    {
        return val;
    }

    Value module = *melM_stackOffset(&vm->stack, 0);
    assert(module.type == MELON_TYPE_OBJECT);

    Value fd;

    if (melStdioAPIGetStdin(vm, &fd) != 0)
    {
        return 1;
    }

    GCItem* stdinObj = melNewObject(vm);
    melM_vstackPushGCItem(&vm->stack, stdinObj);
    melSetValueObject(vm, stdinObj, &fileDescSymbol, &fd);
    melAddItemToObjModule(vm, module.pack.obj, melNewString(vm, "stdin", strlen("stdin")), stdinObj);
    melM_stackPop(&vm->stack);

    if (melStdioAPIGetStderr(vm, &fd) != 0)
    {
        return 1;
    }
    
    GCItem* stderrObj = melNewObject(vm);
    melM_vstackPushGCItem(&vm->stack, stderrObj);
    melSetValueObject(vm, stderrObj, &fileDescSymbol, &fd);
    melAddItemToObjModule(vm, module.pack.obj, melNewString(vm, "stderr", strlen("stderr")), stderrObj);
    melM_stackPop(&vm->stack);

    if (melStdioAPIGetStdout(vm, &fd) != 0)
    {
        return 1;
    }
    
    GCItem* stdoutObj = melNewObject(vm);
    melM_vstackPushGCItem(&vm->stack, stdoutObj);
    melSetValueObject(vm, stdoutObj, &fileDescSymbol, &fd);
    melAddItemToObjModule(vm, module.pack.obj, melNewString(vm, "stdout", strlen("stdout")), stdoutObj);
    melM_stackPop(&vm->stack);
    
    return 0;
}