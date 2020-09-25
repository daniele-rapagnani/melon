#include "melon/modules/json/json_module.h"
#include "melon/modules/modules.h"
#include "melon/core/tstring.h"
#include "melon/core/object.h"
#include "melon/core/array.h"
#include "melon/libs/cJSON/cJSON.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

static Value ioKey;
static Value openFileKey;
static Value readFileKey;

static void deserializeJson(VM* vm, cJSON* node)
{
    static Value tmpValue;

    if (cJSON_IsBool(node))
    {
        tmpValue.type = MELON_TYPE_BOOL;
        tmpValue.pack.value.boolean = node->valueint;
        melM_stackPush(&vm->stack, &tmpValue);
    }
    else if (cJSON_IsNumber(node))
    {
        tmpValue.type = MELON_TYPE_NUMBER;
        tmpValue.pack.value.number = node->valuedouble;
        melM_stackPush(&vm->stack, &tmpValue);
    }
    else if (cJSON_IsNull(node))
    {
        tmpValue.type = MELON_TYPE_NULL;
        melM_stackPush(&vm->stack, &tmpValue);
    }
    else if (cJSON_IsString(node))
    {
        melM_vstackPushGCItem(&vm->stack, melNewString(vm, node->valuestring, strlen(node->valuestring)));
    }
    else if (cJSON_IsArray(node))
    {
        GCItem* arr = melNewArray(vm);
        melM_vstackPushGCItem(&vm->stack, arr);

        cJSON* cur = NULL;
        cJSON_ArrayForEach(cur, node)
        {
            deserializeJson(vm, cur);
            melPushArray(vm, arr, melM_stackPop(&vm->stack));
        }
    }
    else
    {
        assert(cJSON_IsObject(node));
        GCItem* obj = melNewObject(vm);
        melM_vstackPushGCItem(&vm->stack, obj);

        cJSON* cur = NULL;
        cJSON_ArrayForEach(cur, node)
        {
            deserializeJson(vm, cur);
            tmpValue.type = MELON_TYPE_STRING;
            tmpValue.pack.obj = melNewString(vm, cur->string, strlen(cur->string)); 
            melSetValueObject(vm, obj, &tmpValue, melM_stackPop(&vm->stack));
        }
    }
}

static cJSON* serializeJson(VM* vm, Value* node)
{
    assert(node->type != MELON_TYPE_NONE);

    if (node->type == MELON_TYPE_NUMBER)
    {
        return cJSON_CreateNumber(node->pack.value.number);
    }
    else if (node->type == MELON_TYPE_INTEGER)
    {
        return cJSON_CreateNumber(node->pack.value.integer);
    }
    else if (node->type == MELON_TYPE_BOOL)
    {
        return cJSON_CreateBool(node->pack.value.boolean);
    }
    else if (node->type == MELON_TYPE_NULL)
    {
        return cJSON_CreateNull();
    }
    else if (node->type == MELON_TYPE_STRING)
    {
        const char* data = melM_strDataFromObj(node->pack.obj);
        return cJSON_CreateString(data);
    }
    else if (node->type == MELON_TYPE_ARRAY)
    {
        Array* arr = melM_arrayFromObj(node->pack.obj);
        cJSON* arrJson = cJSON_CreateArray();

        for (TSize i = 0; i < arr->count; i++)
        {
            cJSON* item = serializeJson(vm, &arr->data[i]);

            if (item == NULL)
            {
                continue;
            }

            cJSON_AddItemToArray(arrJson, item);
        }

        return arrJson;
    }
    else if (node->type == MELON_TYPE_OBJECT)
    {
        Object* obj = melM_objectFromObj(node->pack.obj);
        cJSON* objJson = cJSON_CreateObject();

        for (TSize i = 0; i < obj->count; i++)
        {
            Value* key = melGetKeyAtIndexObject(vm, node->pack.obj, i, 0);

            if (key->type !=  MELON_TYPE_STRING)
            {
                continue;
            }

            const char* keyStr = melM_strDataFromObj(key->pack.obj);
            Value* value = melGetValueObject(vm, node->pack.obj, key);
            cJSON* valueJson = serializeJson(vm, value);

            if (valueJson == NULL)
            {
                continue;
            }

            cJSON_AddItemToObject(objJson, keyStr, valueJson);
        }

        return objJson;
    }

    return NULL;
}

static TRet parseString(VM* vm, const Value* str)
{
    String* contentStr = melM_strFromObj(str->pack.obj);
    const char* contentData = melM_strDataFromObj(str->pack.obj);

    const char* errPtr = NULL;
    cJSON* json = cJSON_ParseWithLengthOpts(contentData, contentStr->len + 1, &errPtr, 1);

    if (json == NULL)
    {
        melM_stackPop(&vm->stack);
        melM_vstackPushNull(&vm->stack);
        return 1;
    }

    deserializeJson(vm, json);
    cJSON_Delete(json);

    return 1;
}

TByte parseFileFunc(VM* vm)
{
    melM_arg(vm, jsonFile, MELON_TYPE_STRING, 0);

    Value* openFileFunc = melGetClosureFromModule(vm, &ioKey, &openFileKey);
    assert(openFileFunc != NULL);

    melM_stackPush(&vm->stack, openFileFunc);
    melM_stackPush(&vm->stack, jsonFile);

    melCallClosureSyncVM(vm, 1, 0, 1);

    Value fileResult = *melM_stackOffset(&vm->stack, 0);

    if (fileResult.type != MELON_TYPE_OBJECT)
    {
        melM_stackPop(&vm->stack);
        melM_vstackPushNull(&vm->stack);
        return 1;
    }
    
    melSaveAndPauseGC(vm, &vm->gc);
    melM_stackPop(&vm->stack);

    Value* readFileFunc = melGetClosureFromModule(vm, &ioKey, &readFileKey);
    assert(readFileFunc != NULL);

    melM_stackPush(&vm->stack, readFileFunc);
    melM_stackPush(&vm->stack, &fileResult);
    melRestorePauseGC(vm, &vm->gc);

    melCallClosureSyncVM(vm, 1, 0, 1);

    Value contentResult = *melM_stackOffset(&vm->stack, 0);

    if (contentResult.type != MELON_TYPE_STRING)
    {
        melM_stackPop(&vm->stack);
        melM_vstackPushNull(&vm->stack);
        return 1;
    }

    return parseString(vm, &contentResult);
}

TByte parseFunc(VM* vm)
{
    melM_arg(vm, jsonString, MELON_TYPE_STRING, 0);
    return parseString(vm, jsonString);
}

TByte stringifyFunc(VM* vm)
{
    melM_arg(vm, value, MELON_TYPE_NONE, 0);
    melM_argOptional(vm, prettify, MELON_TYPE_BOOL, 1);

    cJSON* obj = serializeJson(vm, value);

    if (obj == NULL)
    {
        melM_vstackPushNull(&vm->stack);
        return 1;
    }

    TBool shouldPrettify = 
        prettify->type == MELON_TYPE_BOOL 
        && prettify->pack.value.boolean
    ;

    char* jsonStr = shouldPrettify ? cJSON_Print(obj) : cJSON_PrintUnformatted(obj);
    melM_vstackPushGCItem(&vm->stack, melNewString(vm, jsonStr, strlen(jsonStr)));

    free(jsonStr);
    cJSON_Delete(obj);

    return 1;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "parse", 1, 0, parseFunc },
    { "parseFile", 1, 0, parseFileFunc },
    { "stringify", 2, 0, stringifyFunc },
    { NULL, 0, 0, NULL }
};

TRet melJsonModuleInit(VM* vm)
{
    if (melCreateKeyModule(vm, "io", &ioKey) != 0)
    {
        return 1;
    }

    if (melCreateKeyModule(vm, "open", &openFileKey) != 0)
    {
        return 1;
    }

    if (melCreateKeyModule(vm, "read", &readFileKey) != 0)
    {
        return 1;
    }
  
    return melNewModule(vm, funcs);
}