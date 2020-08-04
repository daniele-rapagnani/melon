#ifndef __melon__strings_table_h__
#define __melon__strings_table_h__

#include "melon/core/types.h"
#include "melon/core/config.h"

typedef struct StringsEntry
{
    char* string;
    TSize len;
    TSize refCount;
    TSize hash;
#ifndef MELON_STRINGS_TABLE_OPENADDRESSING
    struct StringsEntry* next;
#endif
} StringsEntry;

typedef struct StringsTable
{
    TSize capacity;
    TSize count;

#ifdef MELON_STRINGS_TABLE_OPENADDRESSING
    StringsEntry* strings;
#else
    StringsEntry** strings;
#endif
} StringsTable;

TRet melCreateStringsTable(StringsTable* st);
char* melAddStringStringsTable(StringsTable* st, const char* str, TSize len, TSize hash);
TRet melRemoveStringStringsTable(StringsTable* st, const char* str, TSize len, TSize hash);

#endif // __melon__strings_table_h__