#include "melon/libs/cxxopts/include/cxxopts.hpp"
#include "melon/tools/utils.h"

extern "C" {
#include "melon/modules/cli/cli_module.h"
}

int main(int argc, char** argv) 
{
    const char** argvc = new const char*[argc];
    int argcc = argc;

    memcpy(argvc, argv, sizeof(const char*) * argc);

    cxxopts::Options options("Melon", "Melon VM");

    options.add_options()
        ("f,file", "Input file to run", cxxopts::value<std::string>())
        ("b,bytecode", "Treat the input file as bytecode", cxxopts::value<bool>())
        ("k,benchmark", "Print execution time", cxxopts::value<bool>())
        ("v,version", "Print the version and exit")
        ("o,out", "The output file for the compiled bytecode", cxxopts::value<std::string>());  
    ;

    options.parse_positional({"file"});

    auto result = options.parse(argc, argv);

    if (result.count("v") > 0)
    {
        std::cout << "Melon " MELON_VERSION << std::endl;
        return 0;
    }

    if (result.count("f") <= 0)
    {
        std::cerr << "Please specify a file to execute." << std::endl;
        return 1;
    }

    std::string fileName(result["f"].as<std::string>());

    bool isBytecode = result.count("b") > 0;

    if (result.count("b") == 0)
    {
        if (fileName.size() < 5)
        {
            std::cerr << "WARNING: Can't guess file type. Use --bytecode "
                "if this is a bytecode file. Treating it as source code." << std::endl;
        }

        std::string ext = fileName.substr(fileName.size() - 4) ;

        if (ext == ".mbc")
        {
            isBytecode = true;
        }
    }

    VM vm;
    VMConfig config = {};

    if (melCreateVM(&vm, &config) != 0)
    {
        std::cerr << "Can't create VM" << std::endl;
        return 1;
    }

    if (result.count("o") > 0)
    {
        return melon::utils::compileScript(&vm, fileName, result["o"].as<std::string>());
    }
    
    int rargc = 0;
    const char** rargv = NULL;

    for (int i = 0; i < argcc; i++)
    {
        if (strcmp(argvc[i], "--") == 0 && argcc - i > 0)
        {
            rargc = argcc - (i + 1);
            rargv = (const char**)&argv[i + 1];
        }
    }

    if (rargv != NULL)
    {
        melSetArgs(&vm, rargv, rargc);
    }

    int res = 0;

    if (isBytecode)
    {
        res = melon::utils::runByteCode(&vm, fileName) ? 0 : 1;
    }
    else
    {
        res = melon::utils::runScript(&vm, fileName) ? 0 : 1;
    }

    delete[] argvc;

    return res;
}