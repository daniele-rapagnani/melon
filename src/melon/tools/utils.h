#pragma once

#include <string>

extern "C" {
    #include "melon/core/vm.h"
    #include "melon/core/compiler.h"
}

namespace melon {
namespace utils {

bool readFile(const std::string& fileName, size_t* size, TByte** prog);
bool runScript(VM* vm, const std::string& fileName);
bool compileScript(VM* vm, Compiler* c, const std::string& fileName);
bool compileScript(VM* vm, const std::string& fileName, const std::string& outFileName);
bool runByteCode(VM* vm, const std::string& fileName);

} // namespace utils
} // namespace melon