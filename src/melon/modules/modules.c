#include "melon/modules/modules.h"
#include "melon/core/utils.h"
#include "melon/core/gc.h"
#include "melon/core/tstring.h"
#include "melon/core/compiler.h"
#include "melon/core/object.h"
#include "melon/core/closure.h"
#include "melon/core/program.h"
#include "melon/core/deserializer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

TRet melNewModule(VM* vm, const ModuleFunction* functions)
{
    if (functions == NULL)
    {
        return 1;
    }

    GCItem* libObj = melNewObject(vm);
    melM_vstackPushGCItem(&vm->stack, libObj);

    const ModuleFunction* curFunc = functions;

    melSaveAndPauseGC(vm, &vm->gc);

    while (curFunc->func != NULL)
    {
        GCItem* fObj = melNewFunction(vm);
        Function* f = melM_functionFromObj(fObj);

        f->localsSlots = curFunc->localSlots;
        f->args = curFunc->args;
        f->native = curFunc->func;
        f->method = curFunc->method;
        f->rest = curFunc->rest;

        if (curFunc->name != NULL)
        {
            f->name = melNewString(vm, curFunc->name, strlen(curFunc->name));
        }

        melAddFunctionToObjModule(
            vm, 
            libObj, f->name != NULL ? f->name : MELON_SYMBOLIC_KEYS[MELON_OBJSYM_CALL].pack.obj, 
            fObj
        );

        curFunc++;
    }

    melRestorePauseGC(vm, &vm->gc);

    return 0;
}

struct ErrorContext
{
    GCItem* callback;
    GCItem* file;
    VM* vm;
};

static void errorCb(
    void* ctx, 
    const char* message,
    const char* file,
    TSize line, 
    TSize column
)
{
    struct ErrorContext* errCtx = (struct ErrorContext*)ctx;
    assert(errCtx != NULL);
    assert(errCtx->callback != NULL);
    assert(errCtx->vm != NULL);

    melM_vstackPushGCItem(&errCtx->vm->stack, errCtx->callback);
    melM_vstackPushGCItem(&errCtx->vm->stack, melNewString(errCtx->vm, message, strlen(message)));

    if (file != NULL)
    {
        melM_vstackPushGCItem(&errCtx->vm->stack, melNewString(errCtx->vm, file, strlen(file)));
    }
    else
    {
        melM_vstackPushNull(&errCtx->vm->stack);
    }

    Value tmp;
    tmp.type = MELON_TYPE_INTEGER;

    tmp.pack.value.integer = line;
    melM_stackPush(&errCtx->vm->stack, &tmp);
    tmp.pack.value.integer = column;
    melM_stackPush(&errCtx->vm->stack, &tmp);

    melCallClosureSyncVM(errCtx->vm, 4, 0, 0);
}

GCItem* melCompileSourceModule(VM* vm, GCItem* path, GCItem* src, GCItem* errorCallback)
{
    String* strObj = melM_strFromObj(src);
    const char* strData = melM_strDataFromObj(src);
    const char* pathData = path != NULL ? melM_strDataFromObj(path) : NULL;

    Compiler c;

    if (melCreateCompilerFile(&c, vm, pathData, strData, strObj->len) != 0)
    {
        melM_fatal(vm, "Error while creating compiler.");
        return NULL;
    }

    c.module = 1;

    struct ErrorContext* ctx = NULL;

    if (errorCallback != NULL)
    {
        ctx = malloc(sizeof(struct ErrorContext));
        ctx->vm = vm;
        ctx->callback = errorCallback;

        // The compiler will provide its own file
        ctx->file = NULL;

        c.errorCtx = ctx;
        c.error = errorCb;

        c.lexer.errorCtx = ctx;
        c.lexer.error = errorCb;
    }

    if (melRunCompiler(&c) != 0)
    {
        free(ctx);
        return NULL;
    }

    free(ctx);

    return c.main.func;
}

static void errorDeserializerCb(
    void* ctx, 
    const char* message
)
{
    errorCb(ctx, message, NULL, 0, 0);
}

GCItem* melCompileBytecodeModule(VM* vm, GCItem* path, GCItem* code, GCItem* errorCallback)
{
    Deserializer ds;

    struct ErrorContext* ctx = NULL;

    if (errorCallback != NULL)
    {
        ctx = malloc(sizeof(struct ErrorContext));
        ctx->vm = vm;
        ctx->callback = errorCallback;
        ctx->file = path;

        ds.ctx = ctx;
        ds.error = errorDeserializerCb;
    }

    String* codeStr = melM_strFromObj(code);
    TByte* data = (TByte*)melM_strDataFromObj(code);
    GCItem* prog = NULL;

    const char* pathData = melM_strDataFromObj(path);

    if (melLoadProgramVM(vm, data, codeStr->len, &prog, pathData, &ds) != 0)
    {
        free(ctx);
        return NULL;
    }
    
    free(ctx);

    Program* p = melM_programFromObj(prog);
    return p->main;
}

static TBool isByteCode(VM* vm, GCItem* src)
{
    String* srcStr = melM_strFromObj(src);

    if (srcStr->len < MELON_VM_MAGIC_BYTES_COUNT)
    {
        return 0;
    }

    TByte* data = (TByte*)melM_strDataFromObj(src);

    return memcmp(data, &MAGIC_BYTES, MELON_VM_MAGIC_BYTES_COUNT) == 0;
}

TRet melNewModuleFromSource(VM* vm, GCItem* path, GCItem* src, GCItem* errorCallback)
{
    assert(src->type == MELON_TYPE_STRING);
    assert(path == NULL || path->type == MELON_TYPE_STRING);

    GCItem* mainFunc = NULL;

    if (isByteCode(vm, src))
    {
        mainFunc = melCompileBytecodeModule(vm, path, src, errorCallback);
    }
    else
    {
        mainFunc = melCompileSourceModule(vm, path, src, errorCallback);
    }

    if (mainFunc == NULL)
    {
        return 1;
    }

    // Prevent GCing
    melM_vstackPushGCItem(&vm->stack, mainFunc);

    Value progCl;
    progCl.type = MELON_TYPE_CLOSURE;
    progCl.pack.obj = melNewClosure(vm, mainFunc);

    melM_stackPop(&vm->stack);
    melM_stackPush(&vm->stack, &progCl);

    return 0;
}

TRet melRegisterModule(VM* vm, const char* name, ModuleInitFunc initFunc)
{
    assert(initFunc != NULL);

    initFunc(vm);

    StackEntry* obj = melM_stackOffset(&vm->stack, 0);
    assert(obj->type == MELON_TYPE_OBJECT);

#ifdef _DEBUG_GC
    printf("[Modules] Module registered '%s': %p\n", name, obj->pack.obj);
#endif

    GCItem* nameString = melNewString(vm, name, strlen(name));
    melM_vstackPushGCItem(&vm->stack, nameString); // Avoid GCing

    melAddItemToGlobalModule(vm, nameString, obj->pack.obj);

    melM_stackPop(&vm->stack); // Module name
    melM_stackPop(&vm->stack); // Module obj

    return 0;
}

TRet melRegisterModules(VM* vm, const Module* modules)
{
    const Module* curMod = modules;

    while (curMod->name != NULL)
    {
        if (melRegisterModule(vm, curMod->name, curMod->initFunc) != 0)
        {
            return 1;
        }

        curMod++;
    }

    return 0;
}

TRet melAddValueToObjModule(VM* vm, GCItem* obj, GCItem* name, Value* v)
{
    Value keyName;
    keyName.type = name->type;
    keyName.pack.obj = name;

    return melSetValueObject(vm, obj, &keyName, v);
}

TRet melAddItemToObjModule(VM* vm, GCItem* obj, GCItem* name, GCItem* i)
{
    Value val;
    val.type = i->type;
    val.pack.obj = i;

    return melAddValueToObjModule(vm, obj, name, &val);
}

TRet melAddFunctionToObjModule(VM* vm, GCItem* obj, GCItem* name, GCItem* fn)
{
    GCItem* closure = melNewClosure(vm, fn);
    return melAddItemToObjModule(vm, obj, name, closure);
}

TRet melAddValueToGlobalModule(VM* vm, GCItem* name, Value* v)
{
    return melAddValueToObjModule(vm, vm->global, name, v);
}

TRet melAddFunctionToGlobalModule(VM* vm, GCItem* name, GCItem* fn)
{
    return melAddFunctionToObjModule(vm, vm->global, name, fn);
}

TRet melAddItemToGlobalModule(VM* vm, GCItem* name, GCItem* item)
{
    return melAddItemToObjModule(vm, vm->global, name, item);
}

Value* melGetValueFromGlobalModule(VM* vm, Value* key)
{
    return melGetValueObject(vm, vm->global, key);
}

TRet melCreateKeyModule(VM* vm, const char* key, Value* value)
{
    value->type = MELON_TYPE_STRING;
    value->pack.obj = melNewString(vm, key, strlen(key));
    melAddRootValueGC(vm, &vm->gc, value);

    return 0;
}

Value* melGetTypeFromModule(VM* vm, Value* moduleName, Value* keyName, TType type)
{
    GCItem* module = vm->global;

    if (moduleName->type != MELON_TYPE_NULL)
    {
        Value* moduleVal = melGetValueFromGlobalModule(vm, moduleName);
        module = moduleVal->pack.obj;
    }
    
    if (module == NULL || module->type != MELON_TYPE_OBJECT)
    {
        return NULL;
    }

    Value* val = melGetValueObject(vm, module, keyName);

    if (val == NULL || val->type != type)
    {
        return NULL;
    }

    return val;
}

Value* melGetClosureFromModule(VM* vm, Value* moduleName, Value* funcName)
{
    return melGetTypeFromModule(vm, moduleName, funcName, MELON_TYPE_CLOSURE);
}

Value* melGetArgModule(VM* vm, const char* name, TSize slot, TType type, TBool optional)
{
    Value* arg = melGetLocalVM(vm, melGetTopCallFrameVM(vm), slot, 0);

    if (type != MELON_TYPE_NONE && arg->type != type)
    {
        if (!optional || arg->type != MELON_TYPE_NULL)
        {
            melM_fatal(
                vm, 
                "The %s should be a %s but was a '%s'",
                name,
                MELON_TYPES_NAMES[type],
                MELON_TYPES_NAMES[arg->type]
            );
            
            return NULL;
        }
    }

    return arg;
}

Value* melGetThisModule(VM* vm, const char* name)
{
    Value* arg = melGetLocalVM(vm, melGetTopCallFrameVM(vm), 0, 1);

    if (arg == NULL || arg->type != MELON_TYPE_OBJECT)
    {
        melM_fatal(
            vm, 
            "Can't get this for %s",
            name
        );

        return NULL;
    }

    return arg;
}