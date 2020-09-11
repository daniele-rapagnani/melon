#include "melon/core/config.h"

#ifdef MELON_STRINGS_TABLE_OPENADDRESSING

#include "strings_table.h"
#include "melon/core/config.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#ifndef MELON_STRINGS_TABLE_MAX_CACHE_LEN
#define MELON_STRINGS_TABLE_MAX_CACHE_LEN 256
#endif

#define MELON_STRINGS_TABLE_MIN_CAPACITY 16

#define melM_h1(k, st) \
    ((k) % (st)->capacity)

#define melM_h2(k, st) \
    (7 - ((k) % 7))

#define melM_slot(st, h1, h2, i) \
    (((h1) + (i) * (h2)) % st->capacity)

#define melM_islot(k, st, i) \
    melM_slot(st, melM_h1(k), melM_h2(k), i)

#define melM_isEmpty(e) \
    ((e)->string == EMPTY_STRING)

#define melM_isDeleted(e) \
    ((e)->string == DELETED_STRING)

#define melM_isOccupable(e) \
    (melM_isEmpty(e) || melM_isDeleted(e))

#define melM_visitSlotsBegin(st, hash, entryName) \
    {                                             \
        TSize h1 = melM_h1((hash), (st));         \
        TSize h2 = melM_h2((hash), (st));         \
        TSize slot = h1;                          \
        TSize i = 0;                              \
        StringsEntry *entryName;                  \
                                                  \
        while (1)                                 \
        {                                         \
            entryName = &(st)->strings[slot];

#define melM_visitSlotsEnd(st) \
            i++;                             \
            slot = melM_slot((st), h1, h2, i);     \
        }                                    \
    }

static const char* EMPTY_STRING = NULL;
static const char* DELETED_STRING = (const char*)(~0);

TRet melCreateStringsTable(StringsTable* st)
{
    st->capacity = MELON_STRINGS_TABLE_MIN_CAPACITY;
    st->count = 0;
    st->strings = malloc(sizeof(StringsEntry) * st->capacity);
    assert(st->strings != NULL);
    memset(st->strings, 0, sizeof(StringsEntry) * st->capacity);

    return 0;
}

static void growStringsTable(StringsTable* st)
{
    TSize oldCapacity = st->capacity;
    st->capacity <<= 1;
    st->strings = realloc(st->strings, sizeof(StringsEntry) * st->capacity);
    memset(st->strings + oldCapacity, 0, sizeof(StringsEntry) * oldCapacity);

    TSize newBitMask = (st->capacity - 1) & (~(oldCapacity - 1));

    for(TSize i = 0; i <= oldCapacity; i++)
    {
        if (st->strings[i].hash & newBitMask)
        {
            melM_visitSlotsBegin(st, st->strings[i].hash, entry)
            {
                if (melM_isOccupable(entry))
                {
                    *entry = st->strings[i];
                    st->strings[i].string = NULL;
                    break;
                }
            }
            melM_visitSlotsEnd(st)
        }
    }
}

char* melAddStringStringsTable(StringsTable* st, const char* str, TSize len, TSize hash)
{
    if (st->count + 1 >= st->capacity >> 1)
    {
        growStringsTable(st);
    }

    melM_visitSlotsBegin(st, hash, entry)
    {
        if (melM_isOccupable(entry))
        {
            //printf("[Strings Table] adding: %.*s\n", len, str);

            entry->string = malloc(sizeof(char) * (len + 1));
            assert(entry->string);
            memcpy(entry->string, str, len);
            entry->string[len] = '\0';
            entry->len = len;
            entry->refCount = 1;
            entry->hash = hash;

            st->count++;

            return entry->string;
        }
        else if (entry->len == len && strncmp(entry->string, str, len) == 0)
        {
            entry->refCount++;
            //printf("[Strings Table] found: %.*s, refcount = " MELON_PRINTF_SIZE "\n", len, str, entry->refCount);
            return entry->string;
        }
    }
    melM_visitSlotsEnd(st)

    return NULL;
}

TRet melRemoveStringStringsTable(StringsTable* st, const char* str, TSize len, TSize hash)
{
    melM_visitSlotsBegin(st, hash, entry)
    {
        if (melM_isEmpty(entry))
        {
            return 1;
        }
        else if (
            !melM_isDeleted(entry) 
            && entry->len == len 
            && strncmp(entry->string, str, len) == 0
        )
        {
            assert(entry->refCount > 0);
            entry->refCount--;

            //printf("[Strings Table] removing: %.*s, refcount = " MELON_PRINTF_SIZE "\n", len, str, entry->refCount);

            if (entry->refCount == 0)
            {
                //printf("[Strings Table] freeing: %.*s\n", len, str);
                entry->string = (char*)DELETED_STRING;
            }

            return 0;
        }
    }
    melM_visitSlotsEnd(st)

    return 1;
}

#endif