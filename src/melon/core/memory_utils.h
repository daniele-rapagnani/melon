#ifndef __melon__memory_utils_h__
#define __melon__memory_utils_h__

#include "melon/core/types.h"

TRet melGrowBuffer(void** buffer, TSize* size, TSize itemSize, TSize newSize);
TRet melResizeBuffer(void** buffer, TSize* size, TSize itemSize, TSize newSize);

#endif // __melon__memory_utils_h__