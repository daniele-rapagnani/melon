#ifndef __melon__config_h__
#define __melon__config_h__

#include "melon/core/macros.h"
#include "build_config.h"

#include <stdint.h>

/* General */
#define MELON_VERSION_MAJOR 0
#define MELON_VERSION_MINOR 1
#define MELON_VERSION_PATCH 0
#define MELON_VERSION \
    melM_stringify(MELON_VERSION_MAJOR) \
    "." melM_stringify(MELON_VERSION_MINOR) \
    "." melM_stringify(MELON_VERSION_PATCH)

#if UINTPTR_MAX == UINT32_MAX
#define MELON_32BIT
#define MELON_ARCH_BITS "32"
#elif UINTPTR_MAX == UINT64_MAX
#define MELON_64BIT
#define MELON_ARCH_BITS "64"
#else
#error "Missing UNITPTR_MAX on this platform, can't guess if we are building 32 or 64 bit."
#endif

/* Deserializer */
#define MELON_BYTECODE_VERSION 1
#define MELON_BYTECODE_OPCODE_BITS 7
#define MELON_BYTECODE_OPCODE_MASK 0x7F

/* Buffers */
#define MELON_MIN_BUFFER_SIZE 256

/* Compiler */
#define MELON_COMP_MIN_FUNC_STACK_SIZE 8
#define MELON_COMP_MAX_LOCAL_VARIABLES 256
#define MELON_COMP_MAX_UPVALUES 64
#define MELON_COMP_MIN_LOOP_STACK_SIZE 8
#define MELON_COMP_MAX_PATH_SIZE 4096

/* Errors */
#define MELON_DEFAULT_ERROR_CTX 50
#define MELON_DEFAULT_ERROR_LEN 50

/* Garbage Collector */
#define MELON_GC_MIN_GREY_STACK_CAPACITY 1024
#define MELON_GC_TRIGGER_DELTA 1024
#define MELON_GC_MAJOR_TRIGGER_PERCENT 200
#define MELON_GC_MAJOR_MIN_TRIGGER_SIZE 128
#define MELON_GC_DEFAULT_UNIT_OF_WORK 100
#define MELON_GC_RANGE_ITERATOR_POOL_SIZE 10
#define MELON_GC_STATS_SAMPLES_COUNT 256
#define MELON_GC_STATS_MAX_FILENAME 64
#define MELON_GC_NURSERY_SIZE 256
#define MELON_GC_AGE_MAX 2

/* Objects */
#define MELON_OBJ_MIN_CAPACITY 16

/* VM */
#define MELON_VM_MIN_STACK_SIZE 1024
#define MELON_VM_MAX_FUNC_NAME 512
#define MELON_VM_MAX_SYMBOL_NAME 512
#define MELON_VM_MAX_RANGE_STRING 512
#define MELON_VM_FIRST_SYMBOL_ID 1
#define MELON_VM_ITERATOR_VALUE_PROPERTY "value"
#define MELON_VM_ITERATOR_DONE_PROPERTY "done"
#define MELON_VM_MAX_STRING_HASH_LEN 1024

/* Strings Table */
#define MELON_STRINGS_TABLE_MIN_CAPACITY 16
#define MELON_STRINGS_TABLE_MAX_CACHE_LEN 40
/* #define MELON_STRINGS_TABLE_OPENADDRESSING */

#endif // __melon__config_h__
