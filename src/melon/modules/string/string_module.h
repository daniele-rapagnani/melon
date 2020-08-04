#ifndef __melon__string_module_h__
#define __melon__string_module_h__

#include "melon/core/types.h"
#include "melon/core/vm.h"

TRet melStringModuleInit(VM* vm);
TRet melToString(VM* vm, const Value* se);

#endif // __melon__string_module_h__