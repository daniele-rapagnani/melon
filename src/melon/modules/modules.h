#ifndef __melon__modules_h__
#define __melon__modules_h__

#include "melon/core/types.h"
#include "melon/core/vm.h"

#define melM_this(vm, name) \
    Value* name = melGetThisModule(vm, #name)

#define melM_arg(vm,name, type, slot) \
    Value* name = melGetArgModule(vm, #name, slot, type, 0)

#define melM_argOptional(vm,name, type, slot) \
    Value* name = melGetArgModule(vm, #name, slot, type, 1)

typedef TRet(*ModuleInitFunc)(VM*);

typedef struct Module
{
    const char* name;
    ModuleInitFunc initFunc;
} Module;

typedef struct ModuleFunction
{
    const char* name;
    TUint16 args;
    TUint16 localSlots;
    NativeFunctionPtr func;
    TBool method;
    TBool rest;
} ModuleFunction;

TRet melNewModule(VM* vm, const ModuleFunction* functions);
TRet melNewModuleFromSource(VM* vm, GCItem* path, GCItem* src, GCItem* errorCallback);
TRet melRegisterModule(VM* vm, const char* name, ModuleInitFunc initFunc);
TRet melRegisterModules(VM* vm, const Module* modules);

TRet melCreateKeyModule(VM* vm, const char* key, Value* value);
Value* melGetTypeFromModule(VM* vm, Value* moduleName, Value* keyName, TType type);
Value* melGetClosureFromModule(VM* vm, Value* moduleName, Value* funcName);

TRet melAddValueToObjModule(VM* vm, GCItem* obj, GCItem* name, Value* v);
TRet melAddItemToObjModule(VM* vm, GCItem* obj, GCItem* name, GCItem* i);
TRet melAddFunctionToObjModule(VM* vm, GCItem* obj, GCItem* name, GCItem* fn);
TRet melAddValueToGlobalModule(VM* vm, GCItem* name, Value* v);
TRet melAddFunctionToGlobalModule(VM* vm, GCItem* name, GCItem* fn);
TRet melAddItemToGlobalModule(VM* vm, GCItem* name, GCItem* item);
Value* melGetValueFromGlobalModule(VM* vm, Value* key);

GCItem* melCompileSourceModule(VM* vm, GCItem* path, GCItem* src, GCItem* errorCallback);
GCItem* melCompileBytecodeModule(VM* vm, GCItem* path, GCItem* code, GCItem* errorCallback);

Value* melGetArgModule(VM* vm, const char* name, TSize slot, TType type, TBool optional);
Value* melGetThisModule(VM* vm, const char* name);

#endif // __melon__modules_h__