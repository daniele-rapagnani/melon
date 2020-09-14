#ifndef __melon__system_modules_h__
#define __melon__system_modules_h__

#include "melon/modules/modules.h"

#include "melon/modules/io/io_module.h"
#include "melon/modules/import/import_module.h"
#include "melon/modules/string/string_module.h"
#include "melon/modules/fs/fs_module.h"
#include "melon/modules/path/path_module.h"
#include "melon/modules/math/math_module.h"
#include "melon/modules/json/json_module.h"
#include "melon/modules/object/object_module.h"
#include "melon/modules/types/types_module.h"
#include "melon/modules/compiler/compiler_module.h"
#include "melon/modules/debug/debug_module.h"
#include "melon/modules/cli/cli_module.h"
#include "melon/modules/number/number_module.h"
#include "melon/modules/integer/integer_module.h"
#include "melon/modules/array/array_module.h"
#include "melon/modules/function/function_module.h"
#include "melon/modules/gc/gc_module.h"
#include "melon/modules/random/random_module.h"
#include "melon/modules/system/system_module.h"

#include <stdlib.h>

Module MEL_SYSTEM_MODULES[] = {
    { "io", melIoModuleInit },
    { "fs", melFsModuleInit },
    { "path", melPathModuleInit },
    { "import", melImportModuleInit },
    { "string", melStringModuleInit },
    { "object", melObjectModuleInit },
    { "json", melJsonModuleInit },
    { "math", melMathModuleInit },
    { "types", melTypesModuleInit },
    { "compiler", melCompilerModuleInit },
    { "debug", melDebugModuleInit },
    { "cli", melCliModuleInit },
    { "number", melNumberModuleInit },
    { "integer", melIntegerModuleInit },
    { "array", melArrayModuleInit },
    { "function", melFunctionModuleInit },
    { "gc", melGcModuleInit },
    { "random", melRandomModuleInit },
    { "system", melSystemModuleInit },
    { NULL, NULL }
};

#endif // __melon__system_modules_h__