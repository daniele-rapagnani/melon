#ifndef __melon__stdio_os_api_h__
#define __melon__stdio_os_api_h__

#include "melon/core/value.h"
#include "melon/core/vm.h"

TRet melStdioAPIOpenFile(Value* val, const char* name, const char* flags);
TRet melStdioAPICloseFile(const Value* fd);
TRet melStdioAPIReadFile(VM* vm, const Value* fd, TSize count, Value* result);
TRet melStdioAPIWriteFile(VM* vm, const Value* fd, const Value* data);
TRet melStdioAPIFlushFile(VM* vm, const Value* fd);
TRet melStdioAPITellFile(VM* vm, const Value* fd, Value* result);
TRet melStdioAPISeekFile(VM* vm, const Value* fd, const Value* offset, TBool fromEnd);
TRet melStdioAPISizeFile(VM* vm, const Value* fd, Value* result);
TRet melStdioAPIIsEOFFile(VM* vm, const Value* fd, Value* result);
TRet melStdioAPIGetStdin(VM*vm , Value* result);
TRet melStdioAPIGetStderr(VM*vm , Value* result);
TRet melStdioAPIGetStdout(VM*vm , Value* result);

#endif // __melon__stdio_os_api_h__