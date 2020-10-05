#include "melon/modules/import/import_module.h"
#include "melon/modules/modules.h"
#include "melon/core/utils.h"
#include "melon/core/tstring.h"
#include "melon/core/closure.h"
#include "melon/core/object.h"
#include "melon/core/array.h"

#include <assert.h>
#include <string.h>

static Value cacheKey;
static Value compilerKey;
static Value importKey;
static Value compileFileKey;
static Value importPathsKey;
static Value fsKey;
static Value isFileKey;
static Value realPathKey;
static Value pathKey;
static Value dirnameKey;
static Value namePlaceholder;
static Value modDirPlaceholder;
static Value rootDirPlaceholder;

static TRet resolveModule(VM* vm, Value* curModuleDir, Value* rootDir, GCItem* moduleName)
{
    Value* pathsArray = melGetTypeFromModule(vm, &importKey, &importPathsKey, MELON_TYPE_ARRAY);

    if (pathsArray == NULL)
    {
        melM_vstackPushGCItem(&vm->stack, moduleName);
        return 0;
    }

    Array* pathsArrayArr = melM_arrayFromObj(pathsArray->pack.obj);
    
    if (pathsArrayArr->count == 0)
    {
        melM_vstackPushGCItem(&vm->stack, moduleName);
        return 0;
    }

    Value* existsClosure = melGetClosureFromModule(vm, &fsKey, &isFileKey);

    for (TSize i = 0; i < pathsArrayArr->count; i++)
    {
        Value* val = melGetIndexArray(vm, pathsArray->pack.obj, i);

        if (!val || val->type != MELON_TYPE_STRING)
        {
            continue;
        }

        GCItem* replacedStr = melNewReplaceString(
            vm, 
            val->pack.obj, 
            namePlaceholder.pack.obj,
            moduleName,
            NULL,
            NULL
        );

        melM_vstackPushGCItem(&vm->stack, replacedStr);

        replacedStr = melNewReplaceString(
            vm,
            replacedStr,
            modDirPlaceholder.pack.obj,
            curModuleDir->pack.obj,
            NULL,
            NULL
        );

        // Pops previous temp string
        melM_stackPop(&vm->stack);
        melM_vstackPushGCItem(&vm->stack, replacedStr);

        replacedStr = melNewReplaceString(
            vm,
            replacedStr,
            rootDirPlaceholder.pack.obj,
            rootDir->pack.obj,
            NULL,
            NULL
        );

        melM_stackPop(&vm->stack);

        assert(replacedStr != NULL);

        melM_vstackPushGCItem(&vm->stack, replacedStr);
        melM_stackPush(&vm->stack, existsClosure);
        melM_vstackPushGCItem(&vm->stack, replacedStr);
        melCallClosureSyncVM(vm, 1, 0, 1);

        Value existsResult = *melM_stackPop(&vm->stack);

        if (existsResult.type == MELON_TYPE_BOOL && existsResult.pack.value.boolean)
        {
            Value* realPathClosure = melGetClosureFromModule(vm, &pathKey, &realPathKey);

            // remove replacedStr that's still on the stack
            melM_stackPop(&vm->stack);

            melM_stackPush(&vm->stack, realPathClosure);
            melM_vstackPushGCItem(&vm->stack, replacedStr);
            
            melCallClosureSyncVM(vm, 1, 0, 1);

            Value* realPathResult = melM_stackTop(&vm->stack);

            if (realPathResult->type == MELON_TYPE_NULL)
            {
                melM_stackPopCount(&vm->stack, 1);
                return 1;
            }

            return 0;
        }
    }

    return 1;
}

static TRet pushCachedModule(VM* vm, Value* resolvedModule)
{
    Value* cache = melGetTypeFromModule(vm, &importKey, &cacheKey, MELON_TYPE_OBJECT);

    if (cache == NULL)
    {
        return 1;
    }

    Value* module = melGetValueObject(vm, cache->pack.obj, resolvedModule);

    if (module == NULL)
    {
        return 1;
    }

    melM_stackPush(&vm->stack, module);
    return 0;
}

static TRet storeCachedModule(VM* vm, Value* resolvedModule, Value* module)
{
    Value* cache = melGetTypeFromModule(vm, &importKey, &cacheKey, MELON_TYPE_OBJECT);

    if (cache == NULL)
    {
        return 1;
    }

    melSetValueObject(vm, cache->pack.obj, resolvedModule, module);
    return 0;
}

static const char* getNearestFunctionFile(VM* vm)
{
    for (TSize i = 0; i < vm->callStack.top; i++)
    {
        CallFrame* cf = melM_stackOffset(&vm->callStack, i);

        if (cf->function->debug.file != NULL)
        {
            return cf->function->debug.file;
        }
    }

    return NULL;
}

static const char* getRootFunctionFile(VM* vm)
{
    if (vm->callStack.top == 0)
    {
        return NULL;
    }

    return melM_stackGet(&vm->callStack, 0)->function->debug.file;
}

static void pushPathDirname(VM* vm, const char* path)
{
    Value* dirnameClosure = melGetClosureFromModule(vm, &pathKey, &dirnameKey);

    if (path != NULL)
    {
        melM_stackPush(&vm->stack, dirnameClosure);
        melM_vstackPushGCItem(&vm->stack, melNewString(vm, path, strlen(path)));
        melCallClosureSyncVM(vm, 1, 0, 1);
    }
    else
    {
        melM_vstackPushGCItem(&vm->stack, melNewString(vm, ".", 1));
    }
}

TByte importModuleFunc(VM* vm)
{
    melM_arg(vm, modName, MELON_TYPE_STRING, 0);
    melM_argOptional(vm, customModuleDir, MELON_TYPE_STRING, 1);

    const char* modNameData = melM_strDataFromObj(modName->pack.obj);
    const char* curModuleDirStr = NULL;

    if (customModuleDir->type != MELON_TYPE_NULL)
    {
        curModuleDirStr = melM_strDataFromObj(customModuleDir->pack.obj);
    }
    else
    {
        curModuleDirStr = getNearestFunctionFile(vm);
    }

    pushPathDirname(vm, curModuleDirStr);
    melSetLocalVM(vm, melGetTopCallFrameVM(vm), 2, melM_stackPop(&vm->stack), 0);
    Value* curModuleDir = melGetArgModule(vm, "curModuleDir", 2, MELON_TYPE_STRING, 0);

    pushPathDirname(vm, getRootFunctionFile(vm));
    melSetLocalVM(vm, melGetTopCallFrameVM(vm), 3, melM_stackPop(&vm->stack), 0);
    Value* rootDir = melGetArgModule(vm, "rootDir", 3, MELON_TYPE_STRING, 0);

    if (resolveModule(vm, curModuleDir, rootDir, modName->pack.obj) != 0)
    {
        melM_fatal(vm, "Can't find module '%s'.", modNameData);

        return 0;
    }

    // Move the resolved module value back to modName
    melSetLocalVM(vm, melGetTopCallFrameVM(vm), 1, melM_stackOffset(&vm->stack, 0), 1);
    melM_stackPop(&vm->stack);
    Value* resolvedModule = melGetArgModule(vm, "resolvedModule", 1, MELON_TYPE_STRING, 0);

    if (pushCachedModule(vm, resolvedModule) != 0)
    {
        Value* compileFileFunc = melGetClosureFromModule(vm, &compilerKey, &compileFileKey);
        assert(compileFileFunc != NULL);

        melM_stackPush(&vm->stack, compileFileFunc);
        melM_stackPush(&vm->stack, resolvedModule);

        melCallClosureSyncVM(vm, 1, 0, 1);

        Value* fileResult = melM_stackOffset(&vm->stack, 0);

        if (fileResult->type == MELON_TYPE_NULL)
        {
            melM_fatal(vm, "Can't find requested module '%s'", modNameData);
            return 0;
        }
        else if (fileResult->type != MELON_TYPE_CLOSURE)
        {
            melM_fatal(
                vm, 
                "The compileFile function didn't return a closure, it returned a '%s'.",
                MELON_TYPES_NAMES[fileResult->type]
            );

            return 0;
        }

        StackSize stackTop = vm->stack.top - 1;

        GCItem* closure = melM_stackOffset(&vm->stack, 0)->pack.obj;
        Closure* cl = melM_closureFromObj(closure);
        GCItem* func = cl->fn;

        melPushModuleObjectVM(vm, func);
        melCallClosureSyncVM(vm, 1, 0, 1);

        if (vm->stack.top - stackTop == 0)
        {
            melM_vstackPushNull(&vm->stack);
        }

        storeCachedModule(vm, resolvedModule, melM_stackOffset(&vm->stack, 0));
    }

    return 1;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { NULL, 2, 3, importModuleFunc },
    { NULL, 0, 0, NULL }
};

static const char* DEFAULT_PATHS[] = {
    "##/?\?",
    "##/?\?.ms",
    "##/?\?.mbc",
    "##/?\?/index.ms",
    "##/?\?/index.mbc",
    "$$/mel_seeds/??.ms",
    "$$/mel_seeds/??.mbc",
    "$$/mel_seeds/?\?/index.ms",
    "$$/mel_seeds/?\?/index.mbc",
    "$$/mel_seeds/??",
    "??",
};

static const TSize DEFAULT_PATHS_COUNT = 
    sizeof(DEFAULT_PATHS) / sizeof(const char*)
;

TRet melImportModuleInit(VM* vm)
{
    if (melCreateKeyModule(vm, "cache", &cacheKey) != 0)
    {
        return 1;
    }

    if (melCreateKeyModule(vm, "paths", &importPathsKey) != 0)
    {
        return 1;
    }

    if (melCreateKeyModule(vm, "import", &importKey) != 0)
    {
        return 1;
    }

    if (melCreateKeyModule(vm, "compiler", &compilerKey) != 0)
    {
        return 1;
    }

    if (melCreateKeyModule(vm, "compileFile", &compileFileKey) != 0)
    {
        return 1;
    }

    if (melCreateKeyModule(vm, "fs", &fsKey) != 0)
    {
        return 1;
    }

    if (melCreateKeyModule(vm, "isFile", &isFileKey) != 0)
    {
        return 1;
    }

    if (melCreateKeyModule(vm, "realpath", &realPathKey) != 0)
    {
        return 1;
    }

    if (melCreateKeyModule(vm, "path", &pathKey) != 0)
    {
        return 1;
    }

    if (melCreateKeyModule(vm, "dirname", &dirnameKey) != 0)
    {
        return 1;
    }

    if (melCreateKeyModule(vm, "??", &namePlaceholder) != 0)
    {
        return 1;
    }

    if (melCreateKeyModule(vm, "##", &modDirPlaceholder) != 0)
    {
        return 1;
    }

    if (melCreateKeyModule(vm, "$$", &rootDirPlaceholder) != 0)
    {
        return 1;
    }

    TRet val = melNewModule(vm, funcs);

    if (val != 0)
    {
        return val;
    }

    Value* module = melM_stackOffset(&vm->stack, 0);
    GCItem* pathsArray = melNewArray(vm);
    melM_vstackPushGCItem(&vm->stack, pathsArray);

    melAddItemToObjModule(
        vm, 
        module->pack.obj, 
        importPathsKey.pack.obj, 
        pathsArray
    );

    for (TSize i = 0; i < DEFAULT_PATHS_COUNT; i++)
    {
        GCItem* pathStr = melNewString(vm, DEFAULT_PATHS[i], strlen(DEFAULT_PATHS[i]));

        Value pathStrVal;
        pathStrVal.type = pathStr->type;
        pathStrVal.pack.obj = pathStr;

        melPushArray(vm, pathsArray, &pathStrVal);
    }

    // Pops the array pushed to prevent GCing
    melM_stackPop(&vm->stack);

    GCItem* cacheObj = melNewObject(vm);
    
    melAddItemToObjModule(
        vm,
        module->pack.obj,
        cacheKey.pack.obj,
        cacheObj
    );

    return 0;
}