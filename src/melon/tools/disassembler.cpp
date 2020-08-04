#include "melon/libs/cxxopts/include/cxxopts.hpp"
#include "melon/tools/utils.h"

extern "C" {
    #include "melon/core/opcodes.h"
    #include "melon/core/utils.h"
    #include "melon/core/tstring.h"
}

#include <string>
#include <array>

namespace {

enum class OpcodeType
{
    NONE = 0,
    VOID,
    OP25,
    OP25S,
    OP13
};

struct OpcodeInfo
{
    OpcodeType type;
    std::string name;
};

std::array<OpcodeInfo, MELON_OP_MAX_COUNT> OPCODES = {{
    { OpcodeType::VOID, "NOP" },
    { OpcodeType::VOID, "ADD" },
    { OpcodeType::VOID, "SUB" },
    { OpcodeType::VOID, "MUL" },
    { OpcodeType::VOID, "DIV" },
    { OpcodeType::OP25, "PUSHK" },
    { OpcodeType::VOID, "CONCAT" },
    { OpcodeType::VOID, "HALT" },
    { OpcodeType::VOID, "PUSHNULL" },
    { OpcodeType::VOID, "PUSHTRUE" },
    { OpcodeType::VOID, "PUSHFALSE" },
    { OpcodeType::OP25S, "JMP" },
    { OpcodeType::OP25, "TESTTRUE" },
    { OpcodeType::OP25, "TESTFALSE" },
    { OpcodeType::VOID, "EQ" },
    { OpcodeType::VOID, "LTE" },
    { OpcodeType::VOID, "GTE" },
    { OpcodeType::VOID, "GT" },
    { OpcodeType::VOID, "LT" },
    { OpcodeType::VOID, "NEWOBJ" },
    { OpcodeType::VOID, "SETOBJ" },
    { OpcodeType::VOID, "GETOBJ" },
    { OpcodeType::OP25, "PUSHLOC" },
    { OpcodeType::OP25, "STORELOC" },
    { OpcodeType::OP13, "CALL" },
    { OpcodeType::OP25, "RET" },
    { OpcodeType::OP13, "CALLTAIL" },
    { OpcodeType::OP25S, "PUSHI" },
    { OpcodeType::OP25, "NEWCL" },
    { OpcodeType::OP25, "DUMPINFO" },
    { OpcodeType::OP25, "PUSHUPVAL" },
    { OpcodeType::OP25, "STOREUPVAL" },
    { OpcodeType::VOID, "POP" },
    { OpcodeType::VOID, "GC" },
    { OpcodeType::VOID, "TESTNULL" },
    { OpcodeType::OP25, "PUSH" },
    { OpcodeType::VOID, "NEWARR" },
    { OpcodeType::VOID, "GETARR" },
    { OpcodeType::OP25, "GETIARR" },
    { OpcodeType::VOID, "SETARR" },
    { OpcodeType::OP25, "SETIARR" },
    { OpcodeType::VOID, "PUSHARR" },
    { OpcodeType::VOID, "SIZEARR" },
    { OpcodeType::VOID, "OR" },
    { OpcodeType::VOID, "AND" },
    { OpcodeType::VOID, "NOT" },
    { OpcodeType::VOID, "SETGLOBAL" },
    { OpcodeType::VOID, "GETGLOBAL" },
    { OpcodeType::OP25, "SLICE" },
    { OpcodeType::VOID, "BLESS" },
    { OpcodeType::VOID, "PUSHTHIS" },
    { OpcodeType::VOID, "NEG" },
    { OpcodeType::VOID, "NEWSYMBOL" },
    { OpcodeType::VOID, "PUSHBLESSING" },
    { OpcodeType::OP25, "PUSHITER" },
    { OpcodeType::OP25, "NEXTITER" },
    { OpcodeType::OP13, "SETUPFOR" },
    { OpcodeType::OP25, "BITLSHIFT" },
    { OpcodeType::OP25, "BITRSHIFT" },
    { OpcodeType::OP25, "BITAND" },
    { OpcodeType::OP25, "BITOR" },
    { OpcodeType::OP25, "BITNOT" },
    { OpcodeType::OP25, "BITXOR" },
    { OpcodeType::VOID, "POW" },
    { OpcodeType::VOID, "MOD" },
    { OpcodeType::OP25, "STORETOPCL" },
    { OpcodeType::VOID, "COALESCE" },
}};

}

class Disassembler
{
public:
    void disassembleFunction(GCItem* f)
    {
        Function* fobj = melM_functionFromObj(f);
        size_t size = fobj->size / sizeof(TVMInst);

        std::string name(fobj->name ? melM_strDataFromObj(fobj->name) : "@anonymous@");

        std::cout << std::endl << "--- function: " << name << " ---" << std::endl;
        std::cout << "size: " << fobj->size << " bytes" << std::endl;
        std::cout << "instructions: " << size << std::endl;
        std::cout << "locals: " << fobj->localsSlots << std::endl;
        std::cout << std::endl;

        for (size_t i = 0; i < size; i++)
        {
            Instruction* inst = &(fobj->code[i]);
            TVMOpcode op = static_cast<TVMOpcode>(melM_getOpCode(inst->inst));
            OpcodeInfo opi = OPCODES[op];

            std::cout << (i + 1) << ": " << opi.name;

            switch(opi.type)
            {
                case OpcodeType::OP25:
                    std::cout << "\t" << melM_op_geta25(inst->inst);
                    break;

                case OpcodeType::OP25S:
                    std::cout << "\t" << melM_op_geta25s(inst->inst);
                    break;

                case OpcodeType::OP13:
                    std::cout << "\t" << melM_op_geta13(inst->inst) << "\t" << melM_op_getb12(inst->inst);
                    break;

                default:
                    break;
            }

            if (op == MELON_OP_JMP)
            {
                std::cout << "\t -> " << (i + melM_op_geta25s(inst->inst) + 2);
            }
            else if (op == MELON_OP_PUSHK)
            {
                TVMInstK k = melM_op_geta25(inst->inst);
                Value* v = &fobj->constants.constants[k];
                struct StrFormat sf = melDumpValueUtils(this->vm, v);
                printf("\t -> %.*s", sf.size, sf.buffer);
                continue;
            }

            std::cout << std::endl;
        }

        if (fobj->constants.count > 0)
        {
            std::cout << std::endl << "-- constants:" << std::endl;
        }

        for (size_t i = 0; i < fobj->constants.count; i++)
        {
            struct StrFormat sf = melDumpValueUtils(this->vm, &fobj->constants.constants[i]);
            printf("%d\t\t%.*s", i, sf.size, sf.buffer);
        }

        if (fobj->upvaluesInfos.count > 0)
        {
            std::cout << std::endl << "-- upvalues:" << std::endl;
        }

        for (size_t i = 0; i < fobj->upvaluesInfos.count; i++)
        {
            UpvalueInfo* ui = &fobj->upvaluesInfos.infos[i];
            printf("instack = %s\t idx = %d\n", ui->instack == 1 ? "true" : "false", ui->idx);
        }

        for (size_t i = 0; i < fobj->funcPrototype.count; i++)
        {
            disassembleFunction(fobj->funcPrototype.prototypes[i]);
        }
    }

    void run(VM& vm, const Compiler& c)
    {
        this->vm = &vm;
        disassembleFunction(c.main.func);
    }

private:
    VM* vm;
};

int main(int argc, char** argv) 
{
    cxxopts::Options options("Melon Disassembler", "");

    options.add_options()
        ("f,file", "Input file to run", cxxopts::value<std::string>())
    ;

    options.parse_positional({"file"});

    auto result = options.parse(argc, argv);

    if (result.count("file") == 0)
    {
        std::cerr << "Please specify a file to disassemble." << std::endl;
        return 1;
    }

    std::string fileName(result["file"].as<std::string>());

    VM vm;
    VMConfig config = {};

    if (melCreateVM(&vm, &config) != 0)
    {
        std::cerr << "Can't create VM" << std::endl;
        return 1;
    }

    Compiler c;

    if (!melon::utils::compileScript(&vm, &c, fileName))
    {
        std::cerr << "Can't compile file" << std::endl;
        return 1;
    }

    Disassembler d;
    d.run(vm, c);
}