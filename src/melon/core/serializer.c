#include "melon/core/serializer.h"
#include "melon/core/tstring.h"

#include <stdlib.h>
#include <assert.h>

#define melM_error(s, str) ((s)->error ? (s)->error((s)->ctx, str) : 0)
#define melM_checkWrite(fn) if ((fn) != 0) { return 1; }

TRet melSerializerWriteData(Serializer* s, TByte* b, TSize count)
{
    return s->writer(s->ctx, b, count);
}

TRet melSerializerWriteInst(Serializer* s, TVMInst i)
{
    return melSerializerWriteData(s, (TByte*)&i, sizeof(TVMInst));
}

TRet melSerializerWriteOpVoid(Serializer* s, TVMOpcode o)
{
    return melSerializerWriteInst(s, o);
}

TRet melSerializerWriteOp1_25(Serializer* s, TVMOpcode o, TVMInstK k)
{
    return melSerializerWriteInst(s, melInstMake25(o, k));
}

TRet melSerializerWriteOp1_s25(Serializer* s, TVMOpcode o, TVMInstSK k)
{
    return melSerializerWriteInst(s, melInstMakeS25(o, k));
}

TRet melSerializerWriteOp2_13(Serializer* s, TVMOpcode o, TVMInstK a, TVMInstK b)
{
    return melSerializerWriteInst(s, melInstMake13(o, a, b));
}

TRet melSerializerWriteByte(Serializer* s, TByte b)
{
    return melSerializerWriteData(s, (TByte*)&b, 1);
}

TRet melSerializerWriteInt(Serializer* s, TInteger i)
{
    return melSerializerWriteData(s, (TByte*)&i, sizeof(TInteger));
}

static TRet melSerializerWriteSizePtr(Serializer* s, TSize* sz)
{
    return melSerializerWriteData(s, (TByte*)sz, sizeof(TSize));
}

TRet melSerializerWriteSize(Serializer* s, TSize sz)
{
    return melSerializerWriteSizePtr(s, &sz);
}

TRet melSerializerWriteNum(Serializer* s, TNumber n)
{
    return melSerializerWriteData(s, (TByte*)&n, sizeof(TNumber));
}

static TRet melSerializerWriteString(Serializer* s, GCItem* o)
{
    if (o == NULL)
    {
        melM_checkWrite(melSerializerWriteInt(s, 0));
        return 0;
    }

    String* strObj = melM_strFromObj(o);
    const char* str = melM_strDataFromObj(o);

    melM_checkWrite(melSerializerWriteInt(s, strObj->len));
    
    return melSerializerWriteData(s, (TByte*)str, strObj->len);
}

static TRet melSerializerWriteFunction(Serializer* s, GCItem* obj)
{
    Function* fn = melM_functionFromObj(obj);

    melM_checkWrite(melSerializerWriteInt(s, fn->localsSlots));
    melM_checkWrite(melSerializerWriteByte(s, fn->args));
    melM_checkWrite(melSerializerWriteConstants(s, &fn->constants));
    melM_checkWrite(melSerializerWritePrototypes(s, &fn->funcPrototype));
    melM_checkWrite(melSerializerWriteUpvaluesInfos(s, &fn->upvaluesInfos));
    melM_checkWrite(melSerializerWriteDebugSymbols(s, &fn->debug));
    melM_checkWrite(melSerializerWriteString(s, fn->name));
    melM_checkWrite(melSerializerWriteByte(s, fn->method));
    melM_checkWrite(melSerializerWriteByte(s, fn->rest));
    melM_checkWrite(melSerializerWriteSize(s, fn->size));
    melM_checkWrite(melSerializerWriteData(s, (TByte*)fn->code, fn->size));

    return 0;
}

TRet melSerializerWriteObj(Serializer* s, GCItem* o)
{
    melM_checkWrite(melSerializerWriteByte(s, o->type));

    switch(o->type)
    {
        case MELON_TYPE_STRING:
            return melSerializerWriteString(s, o);
            break;

        case MELON_TYPE_FUNCTION:
            return melSerializerWriteFunction(s, o);
            break;

        default:
            melM_error(s, "Don't know how to serialize object type");
            break;
    }

    return 1;
}

static TRet melSerializerWriteObjPtr(Serializer* s, GCItem** obj)
{
    return melSerializerWriteObj(s, *obj);
}

TRet melSerializerWriteValue(Serializer* s, Value* v)
{
    melM_checkWrite(melSerializerWriteByte(s, v->type));

    if (melM_isGCItem(v))
    {
        return melSerializerWriteObj(s, v->pack.obj);
    }

    switch(v->type)
    {
        case MELON_TYPE_INTEGER:
            return melSerializerWriteInt(s, v->pack.value.integer);

        case MELON_TYPE_NUMBER:
            return melSerializerWriteNum(s, v->pack.value.number);

        case MELON_TYPE_BOOL:
            return melSerializerWriteByte(s, v->pack.value.boolean);

        case MELON_TYPE_NULL:
            return 0;

        default:
            melM_error(s, "Don't know how to serialize value type");
            break;
    }

    return 1;
}

TRet melSerializerWriteUpvalueInfo(Serializer* s, UpvalueInfo* ui)
{
    melM_checkWrite(melSerializerWriteByte(s, ui->instack));
    melM_checkWrite(melSerializerWriteByte(s, ui->idx));
    
    return 0;
}

TRet melSerializerWriteHeader(Serializer* s, ProgramHeader* h)
{
    melM_checkWrite(melSerializerWriteData(s, (TByte*)&MAGIC_BYTES, MELON_VM_MAGIC_BYTES_COUNT));
    melM_checkWrite(melSerializerWriteByte(s, MELON_VERSION_MAJOR));
    melM_checkWrite(melSerializerWriteByte(s, MELON_VERSION_MINOR));
    melM_checkWrite(melSerializerWriteByte(s, MELON_VERSION_PATCH));
    melM_checkWrite(melSerializerWriteByte(s, MELON_BYTECODE_VERSION));
    melM_checkWrite(melSerializerWriteByte(s, (TByte)sizeof(TInteger)));
    melM_checkWrite(melSerializerWriteByte(s, (TByte)sizeof(TNumber)));
    melM_checkWrite(melSerializerWriteInt(s, INT_CHECK_VALUE));
    melM_checkWrite(melSerializerWriteNum(s, NUM_CHECK_VALUE));
    melM_checkWrite(melSerializerWriteData(s, (TByte*)&h->reserved, sizeof(h->reserved)));

    return 0;
}

#define melM_serializerGenerateWriteArrayFunc(name, type, countName, itemsVec, writeFunc) \
    TRet name(Serializer *s, type *a)                                                     \
    {                                                                                     \
        melM_checkWrite(melSerializerWriteSize(s, a->countName));                         \
                                                                                          \
        for (TSize i = 0; i < a->countName; i++)                                          \
        {                                                                                 \
            melM_checkWrite(writeFunc(s, &a->itemsVec[i]));                               \
        }                                                                                 \
                                                                                          \
        return 0;                                                                         \
    }

melM_serializerGenerateWriteArrayFunc(
    melSerializerWriteConstants, 
    StaticConstants, 
    count, 
    constants, 
    melSerializerWriteValue
)

melM_serializerGenerateWriteArrayFunc(
    melSerializerWritePrototypes, 
    FunctionPrototypes, 
    count, 
    prototypes, 
    melSerializerWriteObjPtr
)

melM_serializerGenerateWriteArrayFunc(
    melSerializerWriteUpvaluesInfos, 
    UpvaluesInfos, 
    count, 
    infos, 
    melSerializerWriteUpvalueInfo
)

melM_serializerGenerateWriteArrayFunc(
    melSerializerWriteDebugSymbols, 
    DebugSymbols, 
    count, 
    lines, 
    melSerializerWriteSizePtr
)