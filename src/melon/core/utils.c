#include "melon/core/utils.h"
#include "melon/core/gc_item.h"
#include "melon/core/array.h"
#include "melon/core/tstring.h"
#include "melon/core/symbol.h"
#include "melon/core/closure.h"
#include "melon/core/macros.h"
#include "melon/core/vm.h"
#include "melon/core/memory_utils.h"

#if defined(__MACH__)
#include <mach/clock.h>
#include <mach/mach.h>
#elif defined(unix) || defined(__unix__) || defined(__unix)
#include <time.h>
#include <sys/time.h>
#elif defined(__MINGW32__)
#include <windows.h>
#endif

#ifdef HAVE_UNISTD
#include <unistd.h>
#endif

#ifdef HAVE_EXECINFO
#include <execinfo.h>
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

static const TUint64 NS_IN_SECS = 1000000000;

static void stringFmtResize(struct StrFormat* sf, TSize newSize)
{
    melGrowBuffer((void**)&sf->buffer, &sf->size, sizeof(TByte), sf->used + newSize + 1);
}

void melStringFmtWriteChar(struct StrFormat* sf, char c)
{
    stringFmtResize(sf, sf->used + 1);
    sf->buffer[sf->used++] = c;
}

void melStringFmtWriteCharAt(struct StrFormat* sf, char c, TSize idx)
{
    stringFmtResize(sf, idx + 1);
    sf->buffer[idx] = c;
    sf->used = melM_max(sf->used, idx);
}

void melStringFmtUtils(struct StrFormat* sf, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    melStringFmtVargsUtils(sf, fmt, args);
    va_end(args);
}

void melStringFmtVargsUtils(struct StrFormat* sf, const char* fmt, va_list args)
{
    va_list args2;
    va_copy(args2, args);
    size_t size = vsnprintf(NULL, 0, fmt, args);

    stringFmtResize(sf, size);

    vsnprintf(&sf->buffer[sf->used], sf->size - sf->used, fmt, args2);
    va_end(args2);
    sf->used += size;
}

void melStringFmtFreeUtils(struct StrFormat* sf)
{
    if (sf->buffer != NULL)
    {
        free(sf->buffer);
        sf->buffer = NULL;
    }
}

static void melDumpValueIndexed(StackSize n, const Value* val, struct StrFormat* sf);

static void melDumpStringObj(GCItem* obj, struct StrFormat* sf)
{
    String* strObj = melM_strFromObj(obj);
    const char* str = melM_strDataFromObj(obj);

    melStringFmtUtils(sf, "\"%.*s\"", strObj->len, str);
}

static void melDumpClosureObj(GCItem* obj, struct StrFormat* sf)
{
    Closure* cl = melM_closureFromObj(obj);
    Function* fn = melM_functionFromObj(cl->fn);

    if (fn->name != NULL)
    {
        melDumpStringObj(fn->name, sf);
    }
    else
    {
        melStringFmtUtils(sf, "%s", "@anonymous@");
    }
    
    melStringFmtUtils(sf, " (%p)", fn);

    Upvalue** upvalues = melM_closureUpvaluesFromObj(obj);

    if (fn->upvaluesInfos.count > 0)
    {
        melStringFmtUtils(sf, " { \n  Upvalues:\n");
    }

    for (TSize i = 0; i < fn->upvaluesInfos.count; i++)
    {
        Upvalue* uv = upvalues[i];

        if (uv->value->type != MELON_TYPE_CLOSURE || uv->value->pack.obj != obj)
        {
            melStringFmtUtils(sf, "    - (%s) ", uv->value == &uv->closed ? "closed" : "open");
            melDumpValueIndexed(i, uv->value, sf);
        }
        else
        {
            melStringFmtUtils(sf, "    - self\n");
        }
    }

    if (fn->upvaluesInfos.count > 0)
    {
        melStringFmtUtils(sf, "}");
    }
}

static void melDumpFunction(GCItem* func, struct StrFormat* sf)
{
    Function* fn = melM_functionFromObj(func);

    if (fn->name != NULL)
    {
        melDumpStringObj(fn->name, sf);
    }
    else
    {
        melStringFmtUtils(sf, "%s", "@anonymous@ ");
    }

    melStringFmtUtils(
        sf, 
        " %s:%lld\n", 
        fn->debug.file,
        fn->debug.count > 0 ? fn->debug.lines[0] : 0
    );
}

static void melDumpArrayObj(GCItem* obj, struct StrFormat* sf)
{
    Array* arr = melM_arrayFromObj(obj);
    melStringFmtUtils(sf, "Array(size = %d)", arr->count);
}

static void melDumpValue(const Value* val, struct StrFormat* sf)
{
    switch (val->type)
    {
        case MELON_TYPE_INTEGER:
            melStringFmtUtils(sf, "%ld : TInteger\n", val->pack.value.integer);
            break;

        case MELON_TYPE_NUMBER:
            melStringFmtUtils(sf, "%f : TNumber\n", val->pack.value.number);
            break;

        case MELON_TYPE_BOOL:
            melStringFmtUtils(sf, "%s : TBool\n", val->pack.value.boolean > 0 ? "true" : "false");
            break;

        case MELON_TYPE_OBJECT:
            melStringFmtUtils(
                sf, 
#ifdef _DEBUG_GC
                "Object (%p) : TObject\n", 
                val->pack.obj
#else
                "Object : TObject\n"
#endif
            );
            break;

        case MELON_TYPE_ARRAY:
            melStringFmtUtils(sf, "");
            melDumpArrayObj(val->pack.obj, sf);
            melStringFmtUtils(sf, " : TArray\n");
            break;

        case MELON_TYPE_NULL:
            melStringFmtUtils(sf, "null : TNull\n");
            break;

        case MELON_TYPE_CLOSURE:
            melStringFmtUtils(sf, "");
            melDumpClosureObj(val->pack.obj, sf);
            melStringFmtUtils(sf, " : TClosure\n");
            break;

        case MELON_TYPE_STRING:
            melStringFmtUtils(sf, "");
            melDumpStringObj(val->pack.obj, sf);
            melStringFmtUtils(sf, " : TString\n");
            break;

        case MELON_TYPE_PROGRAM:
            melStringFmtUtils(sf, "Program : TProgram\n");
            break;

        case MELON_TYPE_SYMBOL:
            melStringFmtUtils(sf, "");
            if (melM_symbolFromObj(val->pack.obj)->label != NULL)
            {
                melDumpStringObj(melM_symbolFromObj(val->pack.obj)->label, sf);
            }
            melStringFmtUtils(sf, " %d", melM_symbolFromObj(val->pack.obj)->id);
            melStringFmtUtils(sf, " : TSymbol\n");
            break;

        case MELON_TYPE_FUNCTION:
            melDumpFunction(val->pack.obj, sf);
            break;

        case MELON_TYPE_NONE:
            melStringFmtUtils(sf, "empty\n");
            break;

        default:
            melStringFmtUtils(sf, "%s\n", MELON_TYPES_NAMES[val->type]);
            break;
    }
}

static void melDumpValueIndexed(StackSize n, const Value* val, struct StrFormat* sf)
{
    melStringFmtUtils(sf, "%ld) ", n);
    melDumpValue(val, sf);
}

struct StrFormat melDumpValueUtils(VM* vm, const Value* val)
{
    struct StrFormat sf;
    memset(&sf, 0, sizeof(struct StrFormat));
    melDumpValue(val, &sf);

    return sf;
}

void melPrintValueUtils(VM* vm, const Value* val)
{
    struct StrFormat valueStr = melDumpValueUtils(vm, val);

    if (valueStr.buffer == NULL)
    {
        return;
    }

    melPrintVM(vm, &valueStr);
    melStringFmtFreeUtils(&valueStr);
}

void melPrintGCItemUtils(VM* vm, const GCItem* item)
{
    Value val;
    val.type = item->type;
    val.pack.obj = (GCItem*)item;

    melPrintValueUtils(vm, &val);
}

struct StrFormat melDumpStackUtils(VM* vm)
{
    struct StrFormat sf;
    memset(&sf, 0, sizeof(struct StrFormat));

    // We always have at least the main function pushed on the stack
    // and we don't want to show that. This is mainly to get consistent
    // results for the unit tests otherwise main's function pointer will
    // be printed too, varying at every execution.
    
    if (vm->stack.top == 0)
    {
        return sf;
    }

    melStringFmtUtils(&sf, "Stack top: %ld\n\n", vm->stack.top - 1);
    melStringFmtUtils(&sf, "Stack:\n---\n");

    CallFrame* cf = melGetTopCallFrameVM(vm);
    StackSize i = 0;
    
    for (i = vm->stack.top; i > 1; i--)
    {
        Value* val = melM_stackGet(&vm->stack, i - 1);
        StackSize n = vm->stack.top - i;

        if (vm->callStack.top > 1)
        {
            if (i == cf->stackStart - 1)
            {
                melStringFmtUtils(&sf, "S:------------------\n");
            }
            else if (i == cf->stackBase)
            {
                melStringFmtUtils(&sf, "B:------------------\n");
            }
        }

        melDumpValueIndexed(n, val, &sf);
    }

    if (vm->callStack.top > 1)
    {
        if (i == cf->stackStart - 1)
        {
            melStringFmtUtils(&sf, "S:------------------\n");
        }
        else if (i == cf->stackBase)
        {
            melStringFmtUtils(&sf, "B:------------------\n");
        }
    }

    melStringFmtUtils(&sf, "---\n");

    return sf;
}

void melPrintStackUtils(VM* vm)
{
    struct StrFormat stackDump = melDumpStackUtils(vm);

    if (stackDump.buffer == NULL)
    {
        return;
    }

    melPrintVM(vm, &stackDump);
    melStringFmtFreeUtils(&stackDump);
}

struct StrFormat melDumpCallStackUtils(VM* vm)
{
    struct StrFormat sf;
    memset(&sf, 0, sizeof(struct StrFormat));

    melStringFmtUtils(&sf, "Callstack Depth: %ld\n\n", vm->callStack.top);
    melStringFmtUtils(&sf, "Callstack:\n---\n");

    for (TSize i = vm->callStack.top ; i > 0; i--)
    {
        CallFrame* cf = melM_stackGet(&vm->callStack, i - 1);

        String* nameStr = cf->function->name != NULL ? melM_strFromObj(cf->function->name) : NULL;
        const char* nameCStr = cf->function->name != NULL ? melM_strDataFromObj(cf->function->name) : NULL;

        melStringFmtUtils(
            &sf, 
            "%ld) %.*s (%p) (Args: %d, Locals: %ld) - Start: %d, Base: %d\n",
            i - 1,
            nameStr != NULL ? nameStr->len : strlen("@anonymous@"),
            nameCStr != NULL ? nameCStr : "@anonymous@",
            cf->function,
            cf->function->args,
            cf->function->localsSlots,
            vm->stack.top - cf->stackStart - 1,
            vm->stack.top - cf->stackBase - 1
        );
    }

    melStringFmtUtils(&sf, "---\n");

    return sf;
}

void melPrintCallStackUtils(VM* vm)
{
    struct StrFormat callStackDump = melDumpCallStackUtils(vm);

    if (callStackDump.buffer == NULL)
    {
        return;
    }

    melPrintVM(vm, &callStackDump);
    melStringFmtFreeUtils(&callStackDump);
}

struct StrFormat melDumpUpvaluesUtils(VM* vm)
{
    struct StrFormat sf;
    memset(&sf, 0, sizeof(struct StrFormat));

    Upvalue* i = vm->openUpvalues;
    TSize count = 0;

    melStringFmtUtils(&sf, "Upvalues: \n\n");

    while (i != NULL)
    {
        melStringFmtUtils(&sf, "%ld) %p (%s) ", count, i->value, i->value == &i->closed ? "closed" : "open");
        melDumpValue(i->value, &sf);
        i = i->next;

        count++;
    }

    melStringFmtUtils(&sf, "---\n");

    return sf;
}

void melPrintUpvaluesUtils(VM* vm)
{
    struct StrFormat upvalues = melDumpUpvaluesUtils(vm);

    if (upvalues.buffer == NULL)
    {
        return;
    }

    melPrintVM(vm, &upvalues);
    melStringFmtFreeUtils(&upvalues);

}

struct StrFormat melDumpGCInfoUtils(VM* vm, TBool includeSize)
{
    struct StrFormat sf;
    memset(&sf, 0, sizeof(struct StrFormat));

    if (includeSize)
    {
        melStringFmtUtils(&sf, "GC Allocated Bytes: " MELON_PRINTF_SIZE "\n", vm->gc.usedBytes);
    }

    melStringFmtUtils(&sf, "GC Allocated Objs: " MELON_PRINTF_SIZE "\n", vm->gc.whitesCount);

    return sf;
}

void melPrintGCInfoUtils(VM* vm, TBool includeSize)
{
    struct StrFormat callStackDump = melDumpGCInfoUtils(vm, includeSize);

    if (callStackDump.buffer == NULL)
    {
        return;
    }

    melPrintVM(vm, &callStackDump);
    melStringFmtFreeUtils(&callStackDump);
}

TRet melExtractSourceFragmentUtils(
    struct StrFormat* sf, 
    const char* source, 
    TSize sourceLen, 
    TSize line, 
    TSize col, 
    TSize len, 
    TSize ctxLen
)
{
    size_t curLine = 0;
    size_t curCol = 0;
    size_t curIdx = 0;

    while(curIdx < sourceLen)
    {
        const char* curSource = &source[curIdx];
        if (*curSource == '\n')
        {
            curLine++;
            curCol = 0;
            curIdx++;
            continue;
        }

        if (curCol == col && curLine == line)
        {
            size_t realStart = curIdx - (ctxLen > curCol ? curCol : ctxLen);
            size_t realLen = melM_min(realStart + len, sourceLen - 2) - realStart;

            for (size_t i = realStart; i < curIdx + realLen; i++)
            {
                if (source[i] == '\n')
                {
                    break;
                }

                melStringFmtUtils(sf, "%c", source[i]);
            }

            melStringFmtUtils(sf, "\n");
            
            if (curIdx > realStart)
            {
                for (size_t i = realStart; i < curIdx; i++)
                {
                    melStringFmtUtils(sf, "-");
                }
            }

            melStringFmtUtils(sf, "^");
            break;
        }

        curIdx++;
        curCol++;
    }

    melStringFmtUtils(sf, "\n");

    return 0;
}

void melPrintErrorAtSourceUtils(
    VM* vm,
    const char* type,
    const char* message,
    const char* file,
    const char* source,
    TSize sourceSize,
    TSize line,
    TSize col,
    TSize len,
    TSize ctxLen
)
{
    struct StrFormat sf;
    memset(&sf, 0, sizeof(struct StrFormat));

    melStringFmtUtils(&sf, "%s error - %s\n\n", type, message);
    melStringFmtUtils(&sf, "%s", file != NULL ? file : "Unknown file");

    if (line > 0)
    {
        melStringFmtUtils(&sf, " (at line " MELON_PRINTF_INT ":" MELON_PRINTF_INT "):\n\n", line, col);
    }
    else
    {
        melStringFmtUtils(&sf, " (unknown line):\n\n");
    }
    
    if (source != NULL && sourceSize > 0)
    {
        melExtractSourceFragmentUtils(&sf, source, sourceSize, line - 1, col - 1, len, ctxLen);
    }
    else
    {
        melStringFmtUtils(&sf, "-- Source not available --\n");
    }

    melStringFmtUtils(&sf, "\n");
    melPrintVM(vm, &sf);
    melStringFmtFreeUtils(&sf);
}

struct StrFormat melDumpVMCurrentLocation(VM* vm)
{
    struct StrFormat sf;
    memset(&sf, 0, sizeof(struct StrFormat));

    if (melM_stackIsEmpty(&vm->callStack))
    {
        melStringFmtUtils(&sf, "not running");
        return sf;
    }

    CallFrame* cf = melM_stackTop(&vm->callStack);

    assert(cf->function != NULL);

    if (cf->function->debug.file != NULL && cf->function->debug.lines)
    {
        melStringFmtUtils(
            &sf, 
            "%s:%lld\n", 
            cf->function->debug.file,
            cf->function->debug.count < cf->pc ? cf->function->debug.lines[cf->pc] : 0
        );
    }
    else
    {
        melStringFmtUtils(&sf, "unknown location, pc: " MELON_PRINTF_SIZE "\n", cf->pc);
    }

    return sf;
}

void melPrintVMCurrentLocation(VM* vm)
{
    struct StrFormat sf = melDumpVMCurrentLocation(vm);
    melPrintVM(vm, &sf);
    melStringFmtFreeUtils(&sf);
}

void melPrintNativeStackUtils()
{
#if defined(HAVE_UNISTD) && defined(HAVE_EXECINFO)
    void *array[10];
    size_t size = backtrace(array, 10);
    backtrace_symbols_fd(array, size, STDOUT_FILENO);
#else
    printf("melPrintNativeStackUtils unsupported on this platform\n");
#endif
}

void melVMPrintFunctionUtils(struct StrFormat* sf, void* ctx)
{
    printf("%.*s", sf->used, sf->buffer);
}

#if defined(__MACH__)
static clock_serv_t clockName;
static mach_timespec_t clockTime;
static TBool clockInitialized;
#endif

void melGetTimeHD(MelTimeHD* out)
{
#if defined(__MACH__)
    if (!clockInitialized)
    {
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &clockName);
    }

    if (clock_get_time(clockName, &clockTime) != 0)
    {
        assert(0);
    }

    out->nanoSecs = clockTime.tv_nsec;
    out->secs = clockTime.tv_sec;
#elif defined(unix) || defined(__unix__) || defined(__unix)
    struct timespec ts;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
    out->nanoSecs = ts.tv_nsec;
    out->secs = ts.tv_sec;
#elif defined(__MINGW32__)
    LARGE_INTEGER li;

    if(!QueryPerformanceFrequency(&li))
    {
        assert(0);
    }

    LARGE_INTEGER freq;

    if (!QueryPerformanceFrequency(&freq))
    {
        assert(0);
    }

    LARGE_INTEGER res;
    res.QuadPart = li.QuadPart / freq.QuadPart;
    res.QuadPart *= 1000000000;

    out->secs = res.QuadPart / 1000000000;
    out->nanoSecs = res.QuadPart % 1000000000;
#else
    #error melGetTimeHD is not implemented for this OS
#endif
}

void melGetTimeDiffHD(const MelTimeHD* start, const MelTimeHD* end, MelTimeHD* out)
{
    TUint64 secs = end->secs;
    TUint64 ns = end->nanoSecs;

    if (ns < start->nanoSecs)
    {
        ns += NS_IN_SECS;
        secs--;
    }

    assert(secs >= start->secs);

    out->secs = secs - start->secs;
    out->nanoSecs = ns - start->nanoSecs;
}

TUint64 melGetTimeDiffNs(const MelTimeHD* start, const MelTimeHD* end)
{
    MelTimeHD time;
    melGetTimeDiffHD(start, end, &time);
    return (time.secs * NS_IN_SECS) + time.nanoSecs;
}

static char getWindowsDriveLetter(const char* path, TSize len)
{
    if (len >= 3 && isalpha(path[0]) && path[1] == ':' && path[2] == '\\')
    {
        return toupper(path[0]);
    }

    return '\0';
}

static TBool isWindowsPath(const char* path, TSize len)
{
    char drive = getWindowsDriveLetter(path, len);

    if (drive != '\0')
    {
        return 1;
    }

    for(TSize i = 0; i < len; i++)
    {
        if (path[i] == '/')
        {
            return 0;
        }
    }

    return 1;
}

// Unix path converted to Windows path may have
// three more bytes for the drive letter (eg: C:/)
#define WIN_PATH_EXTRA_BYTES 3

static char* buffer = NULL;
static TSize bufferSize = 0;

const char* melConvertToWindowsPath(const char* path, TSize len, TSize* newLen)
{
    TSize newLenLocal = 0;

    if (newLen == NULL)
    {
        newLen = &newLenLocal;
    }

    *newLen = len;

    if (len == 0)
    {
        return path;
    }

    if (isWindowsPath(path, len))
    {
        return path;
    }
    
    if (buffer == NULL || *newLen > bufferSize)
    {
       if (melGrowBuffer((void**)&buffer, &bufferSize, 1, *newLen + WIN_PATH_EXTRA_BYTES + 1) != 0)
       {
           return NULL;
       }
    }

    TSize startSrcIdx = 0;
    TSize startDstIdx = 0;

    if (path[0] == '/')
    {
        buffer[0] = 'C';
        buffer[1] = ':';
        buffer[2] = '\\';
        
        startDstIdx += WIN_PATH_EXTRA_BYTES;
        startSrcIdx++;
        *newLen += (WIN_PATH_EXTRA_BYTES - 1);
    }

    for (TSize i = 0; i < len - startSrcIdx; i++)
    {
        buffer[startDstIdx + i] = path[startSrcIdx + i] == '/' ? '\\' : path[startSrcIdx + i];
    }

    buffer[*newLen] = '\0';

    return buffer;
}

const char* melConvertFromWindowsPath(const char* path, TSize len, TSize* newLen)
{
    TSize newLenLocal = 0;

    if (newLen == NULL)
    {
        newLen = &newLenLocal;
    }

    *newLen = len;

    if (len == 0)
    {
        return path;
    }

    if (!isWindowsPath(path, len))
    {
        return path;
    }
    
    if (buffer == NULL || len > bufferSize)
    {
       if (melGrowBuffer((void**)&buffer, &bufferSize, 1, *newLen + 1) != 0)
       {
           return NULL;
       }
    }

    char drive = getWindowsDriveLetter(path, len);
    TSize startSrcIdx = 0;
    TSize startDstIdx = 0;

    if (drive == 'C')
    {
        buffer[0] = '/';
        startDstIdx++;
        startSrcIdx += WIN_PATH_EXTRA_BYTES;
        *newLen -= (WIN_PATH_EXTRA_BYTES - 1);
    }

    for (TSize i = 0; i < len - startSrcIdx; i++)
    {
        buffer[startDstIdx + i] = path[startSrcIdx + i] == '\\' ? '/' : path[startSrcIdx + i];
    }

    buffer[*newLen] = '\0';

    return buffer;
}

const char* melConvertToNativePath(const char* path, TSize len, TSize* newLen)
{
#if defined(__MINGW32__) || defined(_MSC_VER)
    return melConvertToWindowsPath(path, len, newLen);
#else
    if (newLen != NULL)
    {
        *newLen = len;
    }

    return path;
#endif
}

const char* melConvertFromNativePath(const char* path, TSize len, TSize* newLen)
{
#if defined(__MINGW32__) || defined(_MSC_VER)
    return melConvertFromWindowsPath(path, len, newLen);
#else
    if (newLen)
    {
        *newLen = len;
    }

    return path;
#endif
}
