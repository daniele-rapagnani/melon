#ifndef __melon__serializer_h__
#define __melon__serializer_h__

#include "melon/core/types.h"
#include "melon/core/gc_item.h"
#include "melon/core/program.h"
#include "melon/core/value.h"
#include "melon/core/vm.h"
#include "melon/core/opcodes.h"

typedef TRet (*SerializerWriter)(void* ctx, const void* data, TSize num);
typedef void (*SerializerError)(void* ctx, const char* message);

typedef struct Serializer
{
    void* ctx;
    SerializerWriter writer;
    SerializerError error;
} Serializer;

TRet melSerializerWriteData(Serializer* s, TByte* b, TSize count);
TRet melSerializerWriteByte(Serializer* s, TByte b);
TRet melSerializerWriteInst(Serializer* s, TVMInst i);
TRet melSerializerWriteOpVoid(Serializer* s, TVMOpcode o);
TRet melSerializerWriteOp1_25(Serializer* s, TVMOpcode o, TVMInstK k);
TRet melSerializerWriteOp1_s25(Serializer* s, TVMOpcode o, TVMInstSK k);
TRet melSerializerWriteOp2_13(Serializer* s, TVMOpcode o, TVMInstK a, TVMInstK b);
TRet melSerializerWriteInt(Serializer* s, TInteger i);
TRet melSerializerWriteSize(Serializer* s, TSize sz);
TRet melSerializerWriteNum(Serializer* s, TNumber n);
TRet melSerializerWriteObj(Serializer* s, GCItem* o);
TRet melSerializerWriteValue(Serializer* s, Value* v);
TRet melSerializerWriteUpvalueInfo(Serializer* s, UpvalueInfo* ui);
TRet melSerializerWriteHeader(Serializer* s, ProgramHeader* h);
TRet melSerializerWriteConstants(Serializer* s, StaticConstants* c);
TRet melSerializerWritePrototypes(Serializer* s, FunctionPrototypes* fp);
TRet melSerializerWriteUpvaluesInfos(Serializer* s, UpvaluesInfos* ui);
TRet melSerializerWriteDebugSymbols(Serializer* s, DebugSymbols* ds);

#endif // __melon__serializer_h__