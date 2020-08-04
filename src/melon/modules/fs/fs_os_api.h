#ifndef __melon__fs_os_api_h__
#define __melon__fs_os_api_h__

#include "melon/core/value.h"
#include "melon/core/vm.h"

TRet melFsAPIPathExists(const Value* path, Value* result);
TRet melFsAPIPathIsFile(const Value* path, Value* result);
TRet melFsAPIPathIsDirectory(const Value* path, Value* result);
TRet melFsAPIPathIsReadable(const Value* path, Value* result);
TRet melFsAPIPathIsWritable(const Value* path, Value* result);

#endif // __melon__fs_os_api_h__