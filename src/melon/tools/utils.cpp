#include "utils.h"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>

extern "C" {
    #include "melon/core/tstring.h"
    #include "melon/core/compiler.h"
    #include "melon/core/utils.h"
}

namespace melon {
namespace utils {

bool readFile(const std::string& fileName, size_t* size, TByte** prog)
{
    std::ifstream input;
    input.open(fileName, std::ios_base::binary | std::ios_base::in);

    if (!input.is_open())
    {
        std::cerr << "Couldn't open input file '" << fileName << "'" << std::endl;
        return false;
    }

    input.seekg(0, std::ios_base::end);
    *size = input.tellg();
    input.seekg(0, std::ios_base::beg);

    *prog = new TByte[*size];
    input.read(reinterpret_cast<char*>(*prog), *size);

    return true;
}

bool runScript(VM* vm, const std::string& fileName)
{
    Compiler c;

    if (!compileScript(vm, &c, fileName))
    {
        return false;
    }

    melAddRootGC(vm, &vm->gc, c.main.func);
    melTriggerGC(vm, &vm->gc);

    // We use the same VM used by the compiler so that
    // string hashes for constants remain valid
    if (melRunMainFunctionVM(vm, c.main.func) != 0)
    {
        delete[] c.lexer.source;
        return false;
    }

    delete[] c.lexer.source;
    return true;
}

bool compileScript(VM* vm, Compiler* c, const std::string& fileName)
{
    TByte* prog = nullptr;
    size_t size;

    if (!readFile(fileName, &size, &prog))
    {
        delete[] prog;
        return false;
    }

    // @TODO: Handle Unicode
    if (melCreateCompilerFile(c, vm, fileName.c_str(), (const char*)prog, size) != 0)
    {
        delete[] prog;
        std::cerr << "Can't create compiler" << std::endl;
        return false;
    }

    if (melRunCompiler(c) != 0)
    {
        delete[] prog;
        return false;
    }

    if (c->hasErrors)
    {
        delete[] prog;
        return false;
    }

    return true;
}

bool compileScript(VM* vm, const std::string& fileName, const std::string& outFileName)
{
    Compiler c;

    if (!compileScript(vm, &c, fileName))
    {
        return false;
    }

    std::ofstream outFile;
    outFile.open(outFileName, std::ios_base::binary | std::ios_base::out);

    if (!outFile.is_open())
    {
        std::cerr << "Couldn't open output file '" << outFileName << "'" << std::endl;
        return false;
    }

    Serializer s;

    s.ctx = &outFile;
    s.error = [] (void* ctx, const char* message) {
        std::cerr << "Serialization error: " << message << std::endl;
    };
    
    s.writer = [] (void* ctx, const void* data, TSize num) -> TRet {
        std::ofstream& outFile = *static_cast<std::ofstream*>(ctx);
        const TByte* inData = static_cast<const TByte*>(data);

        outFile.write(reinterpret_cast<const char*>(inData), num);

        return 0;
    };

    ProgramHeader header;
    melSerializerWriteHeader(&s, &header);
    melSerializerWriteObj(&s, c.main.func);

    outFile.close();

    return true;
}

bool runByteCode(VM* vm, const std::string& fileName)
{
    TByte* prog;
    size_t size;

    if (!readFile(fileName, &size, &prog))
    {
        delete[] prog;
        return false;
    }

    GCItem* program = NULL;

    if (melLoadProgramVM(vm, prog, size, &program, fileName.c_str(), nullptr) != 0)
    {
        delete[] prog;
        return false;
    }

    melAddRootGC(vm, &vm->gc, program);
    melRunProgramVM(vm, program);
    melRemoveRootGC(vm, &vm->gc, program);

    delete[] prog;

    return true;
}

} // namespace utils
} // namespace melon