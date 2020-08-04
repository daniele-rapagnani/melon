#include "melon/modules/math/math_module.h"
#include "melon/modules/modules.h"
#include "melon/core/value.h"
#include "melon/core/object.h"
#include "melon/core/tstring.h"
#include "melon/core/utils.h"

#include <assert.h>
#include <string.h>
#include <math.h>

#define melM_wrapNumFunction1Type(name, resType, member)               \
    static TByte name##Function(VM *vm)                                \
    {                                                                  \
        Value *input = melM_stackOffset(&vm->stack, 0);              \
                                                                       \
        melM_valToNumber(vm, input, numInput);                         \
                                                                       \
        numInput.type = resType;                                       \
        numInput.pack.value.member = name(numInput.pack.value.number); \
                                                                       \
        melM_stackPop(&vm->stack);                                       \
        melM_stackPush(&vm->stack, &numInput);                           \
        return 1;                                                      \
    }

#define melM_wrapNumFunction2Type(name, resType, member)              \
    static TByte name##Function(VM *vm)                               \
    {                                                                 \
        Value *a = melM_stackOffset(&vm->stack, 1);                 \
        Value *b = melM_stackOffset(&vm->stack, 0);                 \
                                                                      \
        melM_valToNumber(vm, a, aInput);                              \
        melM_valToNumber(vm, b, bInput);                              \
                                                                      \
        aInput.type = resType;                                        \
        aInput.pack.value.member =                                    \
            name(aInput.pack.value.number, bInput.pack.value.number); \
                                                                      \
        melM_stackPop(&vm->stack);                                      \
        melM_stackPop(&vm->stack);                                      \
        melM_stackPush(&vm->stack, &aInput);                            \
        return 1;                                                     \
    }

#define melM_wrapNumFunction2(name) melM_wrapNumFunction2Type(name, MELON_TYPE_NUMBER, number)
#define melM_wrapNumFunction1(name) melM_wrapNumFunction1Type(name, MELON_TYPE_NUMBER, number)

melM_wrapNumFunction1(sqrt);
melM_wrapNumFunction1(sin);
melM_wrapNumFunction1(cos);
melM_wrapNumFunction1(tan);
melM_wrapNumFunction1(acos);
melM_wrapNumFunction1(asin);
melM_wrapNumFunction1(atan);
melM_wrapNumFunction2(atan2);
melM_wrapNumFunction1(exp);
melM_wrapNumFunction1(log);
melM_wrapNumFunction1Type(round, MELON_TYPE_INTEGER, integer);
melM_wrapNumFunction1Type(floor, MELON_TYPE_INTEGER, integer);
melM_wrapNumFunction1Type(ceil, MELON_TYPE_INTEGER, integer);
melM_wrapNumFunction1(fabs);
melM_wrapNumFunction2(pow);
melM_wrapNumFunction1Type(isnan, MELON_TYPE_BOOL, boolean);
melM_wrapNumFunction1Type(isinf, MELON_TYPE_BOOL, boolean);

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "sqrt", 1, 0, sqrtFunction },
    { "sin", 1, 0, sinFunction },
    { "cos", 1, 0, cosFunction },
    { "tan", 1, 0, tanFunction },
    { "acos", 1, 0, acosFunction },
    { "asin", 1, 0, asinFunction },
    { "atan", 1, 0, atanFunction },
    { "atan2", 1, 0, atan2Function },
    { "exp", 1, 0, expFunction },
    { "log", 1, 0, logFunction },
    { "round", 1, 0, roundFunction },
    { "floor", 1, 0, floorFunction },
    { "ceil", 1, 0, ceilFunction },
    { "abs", 1, 0, fabsFunction },
    { "pow", 1, 0, powFunction },
    { "isNaN", 1, 0, isnanFunction },
    { "isInfinity", 1, 0, isinfFunction },
    { NULL, 0, 0, NULL }
};

TRet melMathModuleInit(VM* vm)
{
    TRet val = melNewModule(vm, funcs);

    if (val != 0)
    {
        return val;
    }

    Value* module = melM_stackOffset(&vm->stack, 0);
    assert(module->type == MELON_TYPE_OBJECT);

    Value pi;
    pi.type = MELON_TYPE_NUMBER;
    pi.pack.value.number = M_PI;

    melAddValueToObjModule(vm, module->pack.obj, melNewString(vm, "pi", strlen("pi")), &pi);

    return 0;
}