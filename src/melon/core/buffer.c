#include "melon/core/config.h"
#include "melon/core/buffer.h"
#include "melon/core/memory_utils.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

TRet melCreateBuffer(Buffer* b, TSize reserve)
{
    b->buffer = NULL;
    b->size = 0;
    b->reserved = 0;

    return melGrowBuffer((void**)&b->buffer, &b->reserved, sizeof(TByte), MELON_MIN_BUFFER_SIZE);
}

TRet melWriteBuffer(Buffer* b, const TByte* data, TSize len)
{
    if (melGrowBuffer((void**)&b->buffer, &b->reserved, sizeof(TByte), b->size + len) != 0)
    {
        return 1;
    }

    memcpy(b->buffer + b->size, data, len);
    b->size += len;

    return 0;
}

TRet melFreeBuffer(Buffer* b)
{
    if (b->buffer != NULL)
    {
        free(b->buffer);
        b->buffer = NULL;
    }

    return 0;
}