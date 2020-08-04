#include "melon/core/config.h"

#ifndef MELON_STRINGS_TABLE_OPENADDRESSING
#include "strings_table.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#ifndef MELON_STRINGS_TABLE_MIN_CAPACITY
#define MELON_STRINGS_TABLE_MIN_CAPACITY 16
#endif

#define melM_bucket(k, st) \
    ((k) & ((st)->capacity - 1))

TRet melCreateStringsTable(StringsTable* st)
{
    st->capacity = MELON_STRINGS_TABLE_MIN_CAPACITY;
    st->count = 0;
    st->strings = malloc(sizeof(StringsEntry*) * st->capacity);
    assert(st->strings != NULL);
    memset(st->strings, 0, sizeof(StringsEntry*) * st->capacity);

    return 0;
}

static void growStringsTable(StringsTable* st)
{
    TSize oldCapacity = st->capacity;
    st->capacity <<= 1;
    st->strings = realloc(st->strings, sizeof(StringsEntry) * st->capacity);
    memset(st->strings + oldCapacity, 0, sizeof(StringsEntry) * oldCapacity);

    TUint64 newBitMask = (st->capacity - 1) & (~(oldCapacity - 1));
    TSize newBucket;

    for(TSize i = 0; i <= oldCapacity; i++)
    {
        StringsEntry* curNode = st->strings[i];
        StringsEntry* prevNode = NULL;

        while (curNode != NULL)
        {
            if (curNode->hash & newBitMask)
            {
                StringsEntry* movNode = curNode;
                newBucket = melM_bucket(curNode->hash, st);

                if (prevNode != NULL)
                {
                    prevNode->next = movNode->next;
                }
                else
                {
                    st->strings[i] = movNode->next;
                }

                curNode = movNode->next;
                
                StringsEntry* curHead = st->strings[newBucket];
                st->strings[newBucket] = movNode;
                movNode->next = curHead;
            }
            else
            {
                prevNode = curNode;
                curNode = curNode->next;
            }
        }
    }
}

static StringsEntry* findValue(StringsTable* st, TSize bucket, const char* str, TSize len, TSize hash)
{
    StringsEntry* head = st->strings[bucket];

    while (head != NULL)
    {
        if (head->len == len && head->hash == hash && memcmp(str, head->string, len) == 0)
        {
            return head;
        }

        head = head->next;
    }

    return NULL;
}

static StringsEntry* addValue(StringsTable* st, TSize bucket, const char* str, TSize len, TUint64 hash)
{
    StringsEntry* node = malloc(sizeof(StringsEntry));
    assert(node != NULL);
    
    node->string = malloc(sizeof(char) * (len + 1));
    assert(node->string);
    memcpy(node->string, str, len);
    node->string[len] = '\0';
    node->len = len;
    node->refCount = 1;
    node->hash = hash;

    StringsEntry* curHead = st->strings[bucket];
    st->strings[bucket] = node;
    node->next = curHead;

    st->count++;

    if (st->count >= st->capacity)
    {
        growStringsTable(st);
    }

    return node;
}

char* melAddStringStringsTable(StringsTable* st, const char* str, TSize len, TSize hash)
{
    TSize bucket = melM_bucket(hash, st);
    StringsEntry* se = findValue(st, bucket, str, len, hash);

    if (se != NULL)
    {
        se->refCount++;
        //printf("[Strings Table] found: %.*s (%.*s), refcount = %llu\n", se->len, se->string, len, str, se->refCount);
        return se->string;
    }

    //printf("[Strings Table] adding: %.*s\n", len, str);
    se = addValue(st, bucket, str, len, hash);
    return se->string;
}

TRet melRemoveStringStringsTable(StringsTable* st, const char* str, TSize len, TSize hash)
{
    TSize bucket = melM_bucket(hash, st);

    StringsEntry* head = st->strings[bucket];
    StringsEntry** prev = &st->strings[bucket];

    while (head != NULL)
    {
        if (head->string == str)
        {
            assert(head->refCount > 0);
            head->refCount--;

            //printf("[Strings Table] removing: %.*s, refcount = %llu\n", len, str, head->refCount);

            if (head->refCount == 0)
            {
                //printf("[Strings Table] freeing: %.*s\n", len, str);

                *prev = head->next;
                
                assert(st->count > 0);
                st->count--;

    #ifdef _ZERO_MEMORY_ON_FREE_GC
                memset(head, 0, sizeof(StringsEntry));
    #endif

                free(head);
            }

            return 0;
        }

        prev = &head->next;
        head = head->next;
    }

    return 1;
}

#endif