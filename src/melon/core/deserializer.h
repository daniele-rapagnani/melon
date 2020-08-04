#ifndef __melon__deserializer_h__
#define __melon__deserializer_h__

#include "melon/core/types.h"
#include "melon/core/gc_item.h"
#include "melon/core/program.h"
#include "melon/core/value.h"
#include "melon/core/vm.h"

typedef void (*DeserializerError)(void* ctx, const char* message);

typedef struct Deserializer
{
    void* ctx;
    TSize count;
    TSize cur;
    TByte* buffer;
    DeserializerError error;
    VM* vm;
} Deserializer;

TRet melDeserializerReadByte(Deserializer* s, TByte* b);
TRet melDeserializerReadInt(Deserializer* s, TInteger* i);
TRet melDeserializerReadSize(Deserializer* s, TSize* sz);
TRet melDeserializerReadNum(Deserializer* s, TNumber* n);
TRet melDeserializerReadGCItem(Deserializer* s, GCItem** o);
TRet melDeserializerReadValue(Deserializer* s, Value* v);
TRet melDeserializerReadUpvalueInfo(Deserializer* s, UpvalueInfo* ui);
TRet melDeserializerReadHeader(Deserializer* s, ProgramHeader* h);
TRet melDeserializerReadConstants(Deserializer* s, StaticConstants* c);
TRet melDeserializerReadPrototypes(Deserializer* s, FunctionPrototypes* fp);
TRet melDeserializerReadUpvaluesInfos(Deserializer* s, UpvaluesInfos* ui);
TRet melDeserializerReadDebugSymbols(Deserializer* s, DebugSymbols* ds);

#endif // __melon__deserializer_h__