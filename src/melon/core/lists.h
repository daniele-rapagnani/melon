#ifndef __melon__lists_utils_h__
#define __melon__lists_utils_h__

#include <stdlib.h>
#include <assert.h>

#define melM_shiftDLList(head, item, count) \
    {                                      \
        (item)->next = (head);             \
        (item)->prev = NULL;               \
                                           \
        if ((item)->next != NULL)          \
        {                                  \
            (item)->next->prev = item;     \
        }                                  \
                                           \
        (head) = item;                     \
        (count)++;                         \
    }

#define melM_removeDLList(typeName, head, item, count)                              \
    {                                                                               \
        typeName **prevNext = (item)->prev != NULL ? &(item)->prev->next : &(head); \
                                                                                    \
        *prevNext = (item)->next;                                                   \
                                                                                    \
        if ((item)->next != NULL)                                                   \
        {                                                                           \
            (item)->next->prev = (item)->prev;                                      \
        }                                                                           \
                                                                                    \
        assert((count) > 0);                                                        \
        (count)--;                                                                  \
    }

#endif // __melon__lists_utils_h__