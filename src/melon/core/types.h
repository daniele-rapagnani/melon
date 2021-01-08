#ifndef __melon__types_h__
#define __melon__types_h__

#include "melon/core/config.h"

#include <stdint.h>
#include <stddef.h>

typedef uint8_t TUint8;
typedef int8_t TInt8;
typedef uint16_t TUint16;
typedef int16_t TInt16;
typedef uint32_t TUint32;
typedef int32_t TInt32;
typedef uint64_t TUint64;
typedef int64_t TInt64;

typedef TUint8 TByte;
typedef TInt8 TRet;
typedef TUint8 TBool;
typedef size_t TSize;

typedef TUint8 TType;
#ifdef MELON_64BIT
typedef TInt64 TInteger;
typedef TUint64 TUInteger;
#else
typedef TInt32 TInteger;
typedef TUint32 TUInteger;
#endif

typedef double TNumber;

typedef TSize TConstantID;
typedef uintptr_t TPtr;
typedef TUint32 TVMInst;
typedef TUint32 TVMInstK;
typedef TInt32 TVMInstSK;
typedef TUint64 TSymbolID;

#ifdef MELON_64BIT
#define MELON_INT_MIN INT64_MIN
#define MELON_INT_MAX INT64_MAX
#else
#define MELON_INT_MIN INT32_MIN
#define MELON_INT_MAX INT32_MAX
#endif

#define MELON_INST_MAX_SIGNED_K (0xFFFFFF)

#ifdef MELON_64BIT
#define MELON_MAX_SYMBOL_ID UINT64_MAX
#else
#define MELON_MAX_SYMBOL_ID UINT32_MAX
#endif

#ifdef MELON_64BIT
#define MELON_PRINTF_INT "%lld"
#define MELON_PRINTF_UINT "%llu"
#else
#define MELON_PRINTF_INT "%ld"
#define MELON_PRINTF_UINT "%lu"
#endif

#define MELON_PRINTF_SIZE MELON_PRINTF_UINT

#define melM_strtonum(x) strtod(x, NULL)

#endif // __melon__types_h__