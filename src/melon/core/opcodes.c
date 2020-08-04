#include "melon/core/opcodes.h"

TVMInst melInstMakeVoid(TVMOpcode op)
{
    return op;
}

TVMInst melInstMake25(TVMOpcode op, TVMInstK k)
{
    return (op & MELON_BYTECODE_OPCODE_MASK) | (k << MELON_BYTECODE_OPCODE_BITS);
}

TVMInst melInstMakeS25(TVMOpcode op, TVMInstSK k)
{
    return melInstMake25(op, MELON_INST_MAX_SIGNED_K + k);
}

TVMInst melInstMake13(TVMOpcode op, TVMInstK a, TVMInstK b)
{
    return 
        (op & MELON_BYTECODE_OPCODE_MASK) 
        | ((a & 0x1FFF) << MELON_BYTECODE_OPCODE_BITS) 
        | ((b & 0xFFF) << (MELON_BYTECODE_OPCODE_BITS + 13))
    ;
}