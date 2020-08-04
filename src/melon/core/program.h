#ifndef __melon__program_h__
#define __melon__program_h__

#include "melon/core/types.h"
#include "melon/core/gc_item.h"
#include "melon/core/vm.h"

#define melM_programFromObj(obj) ((Program*)((TPtr)(obj) + sizeof(GCItem)))

#pragma pack(push, 1)
typedef struct ProgramHeader
{
    TByte   majorVersion;
    TByte   minorVersion;
    TByte   patchVersion;
    TByte   byteCodeVersion;
    TByte   intSize;
    TByte   numSize;
    TInteger intCheck;
    TNumber  numCheck;
    TByte   reserved[10];
} ProgramHeader;

typedef struct Program {
    ProgramHeader header;
    GCItem* main;
} Program;
#pragma pack(pop)

GCItem* melNewProgram(VM* vm);
TRet melFreeProgram(VM* vm, GCItem* s);

#endif // __melon__program_h__