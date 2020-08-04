#ifndef __melon__opcodes_h__
#define __melon__opcodes_h__

#include "melon/core/types.h"

typedef enum {
    MELON_OP_NOP = 0,
    MELON_OP_ADD = 1,
    MELON_OP_SUB = 2,
    MELON_OP_MUL = 3,
    MELON_OP_DIV = 4,
    MELON_OP_PUSHK = 5,
    MELON_OP_CONCAT = 6,
    MELON_OP_HALT = 7,
    MELON_OP_PUSHNULL = 8,
    MELON_OP_PUSHTRUE = 9,
    MELON_OP_PUSHFALSE = 10,
    MELON_OP_JMP = 11,
    MELON_OP_TESTTRUE = 12,
    MELON_OP_TESTFALSE = 13,
    MELON_OP_EQ = 14,
    MELON_OP_LTE = 15,
    MELON_OP_GTE = 16,
    MELON_OP_GT = 17,
    MELON_OP_LT = 18,
    MELON_OP_NEWOBJ = 19,
    MELON_OP_SETOBJ = 20,
    MELON_OP_GETOBJ = 21,
    MELON_OP_PUSHLOC = 22,
    MELON_OP_STORELOC = 23,
    MELON_OP_CALL = 24,
    MELON_OP_RET = 25,
    MELON_OP_CALLTAIL = 26,
    MELON_OP_PUSHI = 27,
    MELON_OP_NEWCL = 28,
    MELON_OP_DUMPINFO = 29,
    MELON_OP_PUSHUPVAL = 30,
    MELON_OP_STOREUPVAL = 31,
    MELON_OP_POP = 32,
    MELON_OP_GC = 33,
    MELON_OP_TESTNULL = 34,
    MELON_OP_PUSH = 35,
    MELON_OP_NEWARR = 36,
    MELON_OP_GETARR = 37,
    MELON_OP_GETIARR = 38,
    MELON_OP_SETARR = 39,
    MELON_OP_SETIARR = 40,
    MELON_OP_PUSHARR = 41,
    MELON_OP_SIZEARR = 42,
    MELON_OP_OR = 43,
    MELON_OP_AND = 44,
    MELON_OP_NOT = 45,
    MELON_OP_SETGLOBAL = 46,
    MELON_OP_GETGLOBAL = 47,
    MELON_OP_SLICE = 48,
    MELON_OP_BLESS = 49,
    MELON_OP_PUSHTHIS = 50,
    MELON_OP_NEG = 51,
    MELON_OP_NEWSYMBOL = 52,
    MELON_OP_PUSHBLESSING = 53,
    MELON_OP_PUSHITER = 54,
    MELON_OP_NEXTITER = 55,
    MELON_OP_SETUPFOR = 56,
    MELON_OP_BITLSHIFT = 57,
    MELON_OP_BITRSHIFT = 58,
    MELON_OP_BITAND = 59,
    MELON_OP_BITOR = 60,
    MELON_OP_BITNOT = 61,
    MELON_OP_BITXOR = 62,
    MELON_OP_POW = 63,
    MELON_OP_MOD = 64,
    MELON_OP_STORETOPCL = 65,
    MELON_OP_COALESCE = 66,

    MELON_OP_MAX_COUNT = (1 << 7)
} TVMOpcode;

#define melM_getOpCode(inst) ((inst) & MELON_BYTECODE_OPCODE_MASK)
#define melM_op_geta25(i) (i >> MELON_BYTECODE_OPCODE_BITS)
#define melM_op_geta25s(i) (TVMInstSK)((i >> MELON_BYTECODE_OPCODE_BITS) - MELON_INST_MAX_SIGNED_K)
#define melM_op_geta13(i) ((i >> MELON_BYTECODE_OPCODE_BITS) & 0x1FFF)
#define melM_op_getb12(i) ((i >> (MELON_BYTECODE_OPCODE_BITS + 13)) & 0x1FFF)

TVMInst melInstMakeVoid(TVMOpcode op);
TVMInst melInstMake25(TVMOpcode op, TVMInstK k);
TVMInst melInstMakeS25(TVMOpcode op, TVMInstSK k);
TVMInst melInstMake13(TVMOpcode op, TVMInstK a, TVMInstK b);

#endif // __melon__opcodes_h__