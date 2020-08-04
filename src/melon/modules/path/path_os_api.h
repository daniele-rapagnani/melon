#ifndef __melon__path_os_api_h__
#define __melon__path_os_api_h__

#include "melon/core/value.h"
#include "melon/core/vm.h"

TRet melPathAPIDirname(VM* vm, const Value* path, Value* result);
TRet melPathAPIBasename(VM* vm, const Value* path, Value* result);
TRet melPathAPIRealpath(VM* vm, const Value* path, Value* result);

#endif // __melon__path_os_api_h__