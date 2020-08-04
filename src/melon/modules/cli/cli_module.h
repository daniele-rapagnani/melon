#ifndef __melon__cli_module_h__
#define __melon__cli_module_h__

#include "melon/core/types.h"
#include "melon/core/vm.h"

TRet melSetArgs(VM* vm, const char** eargv, int eargc);

TRet melCliModuleInit(VM* vm);

#endif // __melon__cli_module_h__