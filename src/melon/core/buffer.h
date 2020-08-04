#ifndef __melon__buffer_h__
#define __melon__buffer_h__

#include "melon/core/types.h"

typedef struct Buffer
{
    TByte* buffer;
    TSize size;
    TSize reserved;
} Buffer;

TRet melCreateBuffer(Buffer* b, TSize reserve);
TRet melWriteBuffer(Buffer* b, const TByte* data, TSize len);
TRet melFreeBuffer(Buffer* b);

#endif /* __buffer_h__ */
