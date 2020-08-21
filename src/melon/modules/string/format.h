/**
 * Based on Marco Paland's tiny printf.
 * https://github.com/mpaland/printf
 */

#ifndef __melon__format_h__
#define __melon__format_h__

#include "melon/core/types.h"
#include "melon/core/vm.h"

TRet melFormat(VM* vm, Value* outString, GCItem* format, GCItem* params);

#endif // __melon__format_h__

