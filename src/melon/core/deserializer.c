#include "melon/core/config.h"
#include "melon/core/deserializer.h"
#include "melon/core/tstring.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define melM_error(s, str) ((s)->error ? (s)->error((s)->ctx, str) : 0)
#define melM_readType(s, ptr, T) melDeserializerReadData(s, (void*)ptr, sizeof(T))
#define melM_readVal(s, val) melDeserializerReadData(s, (void*)&val, sizeof(val))
#define melM_checkRead(fn) if (fn != 0) { return 1; }

static int consts = 0;

static TRet melDeserializerReadData(Deserializer* s, char* d, TSize size)
{
    if (s->cur >= s->count)
    {
        return 1;
    }

    memcpy(d, &s->buffer[s->cur], size);
    s->cur += size;

    return 0;
}

TRet melDeserializerReadByte(Deserializer* s, TByte* b)
{
    *b = s->buffer[s->cur++];
    return 0;
}

TRet melDeserializerReadInt(Deserializer* s, TInteger* i)
{
    return melM_readType(s, i, TInteger);
}

TRet melDeserializerReadSize(Deserializer* s, TSize* sz)
{
    return melM_readType(s, sz, TSize);
}

TRet melDeserializerReadNum(Deserializer* s, TNumber* n)
{
    return melM_readType(s, n, TNumber);
}

static TRet melDeserializerReadString(Deserializer* s, GCItem** o)
{
    TInteger strLen;
    melM_checkRead(melM_readVal(s, strLen));

    *o = melNewString(s->vm, (const char*)&s->buffer[s->cur], strLen);
    s->cur += strLen;

    return 0;
}

static TRet melDeserializerReadFunction(Deserializer* s, GCItem** obj)
{
    *obj = melNewFunction(s->vm);
    Function* f = melM_functionFromObj(*obj);

    melM_checkRead(melDeserializerReadInt(s, &f->localsSlots));
    melM_checkRead(melDeserializerReadByte(s, &f->args));
    melM_checkRead(melDeserializerReadConstants(s, &f->constants));
    melM_checkRead(melDeserializerReadPrototypes(s, &f->funcPrototype));
    melM_checkRead(melDeserializerReadUpvaluesInfos(s, &f->upvaluesInfos));
    melM_checkRead(melDeserializerReadDebugSymbols(s, &f->debug));
    melM_checkRead(melDeserializerReadString(s, &f->name));
    melM_checkRead(melDeserializerReadByte(s, &f->method));
    melM_checkRead(melDeserializerReadByte(s, &f->rest));
    melM_checkRead(melDeserializerReadSize(s, &f->size));

    f->debug.source = NULL;
    f->debug.sourceSize = 0;

    f->native = NULL;
    f->code = malloc(f->size * sizeof(TVMInst));
    assert(f->code != NULL);

    if (f->code == NULL)
    {
        melM_error(s, "Can't allocate memory for a function's prototype code");
        return 1;
    }

    melM_checkRead(melDeserializerReadData(s, (char *)f->code, f->size));

    return 0;
}

TRet melDeserializerReadGCItem(Deserializer* s, GCItem** o)
{
    TByte type;
    melM_checkRead(melDeserializerReadByte(s, &type));

    switch(type)
    {
        case MELON_TYPE_STRING:
            return melDeserializerReadString(s, o);
        
        case MELON_TYPE_FUNCTION:
            return melDeserializerReadFunction(s, o);

        default:
            melM_error(s, "Don't know how to deserialize object type");
            break;
    }

    return 1;
}

TRet melDeserializerReadValue(Deserializer* s, Value* v)
{   
    consts++;

    melM_checkRead(melDeserializerReadByte(s, &v->type));

    if (melM_isGCItem(v))
    {
        return melDeserializerReadGCItem(s, &v->pack.obj);
    }

    switch(v->type)
    {
        case MELON_TYPE_INTEGER:
            return melDeserializerReadInt(s, &v->pack.value.integer);
            break;

        case MELON_TYPE_NUMBER:
            return melDeserializerReadNum(s, &v->pack.value.number);
            break;     

        case MELON_TYPE_BOOL:
            return melDeserializerReadByte(s, &v->pack.value.boolean);
            break;      

        case MELON_TYPE_NULL:
            break;            

        default:
            melM_error(s, "Don't know how to deserialize value type");
            return 1;
    }

    return 1;
}

TRet melDeserializerReadUpvalueInfo(Deserializer* s, UpvalueInfo* ui)
{
    melM_checkRead(melDeserializerReadByte(s, &ui->instack));
    melM_checkRead(melDeserializerReadByte(s, &ui->idx));
    
    return 0;
}

TRet melDeserializerReadHeader(Deserializer* s, ProgramHeader* h)
{
    TByte magic[MELON_VM_MAGIC_BYTES_COUNT];

    melM_checkRead(melM_readVal(s, magic));

    if (memcmp(&magic, &MAGIC_BYTES, MELON_VM_MAGIC_BYTES_COUNT) != 0)
    {
        melM_error(s, "Invalid program file (magic header does not match, maybe the file is corrupt?).");
        return 1;
    }

    melM_checkRead(melDeserializerReadByte(s, &h->majorVersion));
    melM_checkRead(melDeserializerReadByte(s, &h->minorVersion));
    melM_checkRead(melDeserializerReadByte(s, &h->patchVersion));
    melM_checkRead(melDeserializerReadByte(s, &h->byteCodeVersion));

    if (
        h->majorVersion > MELON_VERSION_MAJOR
        || (h->majorVersion == MELON_VERSION_MAJOR && h->minorVersion > MELON_VERSION_MINOR)
        || (
            h->majorVersion == MELON_VERSION_MAJOR 
            && h->minorVersion == MELON_VERSION_MINOR 
            && h->patchVersion > MELON_VERSION_PATCH
        )
    )
    {
        melM_error(s, "Your version is too old to run this bytecode.");
        return 1;
    }

    if (h->byteCodeVersion != MELON_BYTECODE_VERSION)
    {
        melM_error(s, "The bytecode version does not match the VM version.");
        return 1;
    }
    
    melM_checkRead(melDeserializerReadByte(s, &h->intSize));
    melM_checkRead(melDeserializerReadByte(s, &h->numSize));
    melM_checkRead(melDeserializerReadInt(s, &h->intCheck));
    melM_checkRead(melDeserializerReadNum(s, &h->numCheck));
    melM_checkRead(melM_readVal(s, h->reserved));

    if (h->intSize != sizeof(TInteger))
    {
        melM_error(
            s, 
            "The bytecode uses a different integer size. "
            "This probably means you are loading a bytecode compiled "
            "with a 32bit compiler with a 64bit compiler or vice-versa."
        );
        return 1;
    }

    if (h->numSize != sizeof(TNumber))
    {
        melM_error(s, "The bytecode uses a different floating point value size.");
        return 1;
    }

    if (h->intCheck != INT_CHECK_VALUE)
    {
        melM_error(s, "The bytecode seems to be compiled for a different endianness.");
        return 1;
    }

    if (h->numCheck != NUM_CHECK_VALUE)
    {
        melM_error(s, "The bytecode seems to be compiled for a VM with different floating point format.");
        return 1;
    }

    return 0;
}

#define melM_deserializerGenReadArray(name, type, countName, reservName, itemsVec, itemsType, readFunc) \
    TRet name(Deserializer *s, type *a)                  \
    {                                                                            \
        TSize count;                                                             \
        melM_checkRead(melDeserializerReadSize(s, &count));                      \
                                                                                 \
        a->countName = a->reservName = count;                                    \
                                                                                 \
        if (a->countName == 0)                                                   \
        {                                                                        \
            a->itemsVec = NULL;                                                  \
            return 0;                                                            \
        }                                                                        \
                                                                                 \
        a->itemsVec = malloc(a->countName * sizeof(itemsType));                  \
        assert(a->itemsVec != NULL);                                             \
                                                                                 \
        if (a->itemsVec == NULL)                                                 \
        {                                                                        \
            melM_error(s, "Can't allocate memory for the " #itemsType " vector");\
            return 1;                                                            \
        }                                                                        \
                                                                                 \
        for (TSize i = 0; i < a->countName; i++)                                 \
        {                                                                        \
            melM_checkRead(readFunc(s, &a->itemsVec[i]));                        \
        }                                                                        \
                                                                                 \
        return 0;                                                                \
    }

melM_deserializerGenReadArray(
    melDeserializerReadConstants,
    StaticConstants,
    count,
    reserved,
    constants,
    Value,
    melDeserializerReadValue
)

melM_deserializerGenReadArray(
    melDeserializerReadPrototypes,
    FunctionPrototypes,
    count,
    reserved,
    prototypes,
    GCItem*,
    melDeserializerReadGCItem
)

melM_deserializerGenReadArray(
    melDeserializerReadUpvaluesInfos,
    UpvaluesInfos,
    count,
    reserved,
    infos,
    UpvalueInfo,
    melDeserializerReadUpvalueInfo
)

melM_deserializerGenReadArray(
    melDeserializerReadDebugSymbols,
    DebugSymbols,
    count,
    reserved,
    lines,
    TSize,
    melDeserializerReadSize
)
