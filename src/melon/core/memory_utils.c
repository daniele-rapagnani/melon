#include "melon/core/memory_utils.h"
#include "melon/core/macros.h"

#include <stdlib.h>
#include <assert.h>

TRet melGrowBuffer(void** buffer, TSize* size, TSize itemSize, TSize newSize)
{
    if (*size >= newSize)
    {
        return 0;
    }

    return melResizeBuffer(buffer, size, itemSize, melM_max((*size) * 2, newSize));
}

TRet melResizeBuffer(void** buffer, TSize* size, TSize itemSize, TSize newSize)
{
    if (*size == newSize)
    {
        return 0;
    }

    if (newSize == 0)
    {
        if (*buffer != NULL)
        {
            free(*buffer);
            *buffer = NULL;
        }

        *size = 0;

        return 0;
    }

    *buffer = realloc(*buffer, newSize * itemSize);

    if (*buffer == NULL)
    {
        assert(0);
        *buffer = NULL;
        return 1;
    }

    *size = newSize;

    return 0;
}