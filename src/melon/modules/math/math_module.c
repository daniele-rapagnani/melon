#include "melon/modules/math/math_module.h"
#include "melon/modules/modules.h"
#include "melon/core/value.h"
#include "melon/core/object.h"
#include "melon/core/tstring.h"
#include "melon/core/utils.h"

/***
 * @module
 * 
 * This module groups together basic mathematical functions.
 * 
 * @exports pi Represents the ratio of the circumference of a circle to its diameter.
 */

#include <assert.h>
#include <string.h>

#define _USE_MATH_DEFINES
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

/***
 * @function sqrt
 * 
 * Performs the square root of `n`.
 * 
 * @arg n The square root argument
 * @returns The square root of `n`
 */

melM_wrapNumFunction1(sqrt);

/***
 * @function sin
 * 
 * Performs the sine function of angle `a`.
 * 
 * @arg a The angle in radians
 * @returns The sine of `a`
 */

melM_wrapNumFunction1(sin);

/***
 * @function cos
 * 
 * Performs the cosine function of angle `a`.
 * 
 * @arg a The angle in radians
 * @returns The cosine of `a`
 */

melM_wrapNumFunction1(cos);

/***
 * @function tan
 * 
 * Performs the tangent function of angle `a`.
 * 
 * @arg a The angle in radians
 * @returns The tangent of `a`
 */

melM_wrapNumFunction1(tan);

/***
 * @function acos
 * 
 * Performs the arccosine of `n`.
 * 
 * @arg n The argument to the arccosine function
 * @returns The arccosine of `n`
 */

melM_wrapNumFunction1(acos);

/***
 * @function asin
 * 
 * Performs the arcsine of `n`.
 * 
 * @arg n The argument to the arcsine function
 * @returns The arcsine of `n`
 */

melM_wrapNumFunction1(asin);

/***
 * @function atan
 * 
 * Performs the arctangent of `n`.
 * 
 * @arg n The argument to the arctangent function
 * @returns The arctangent of `n`
 */

melM_wrapNumFunction1(atan);

/***
 * @function atan2
 * 
 * Performs the arctangent2 of `x` and `y`.
 * 
 * @arg x The x coordinates of the point
 * @arg y The y coordinates of the point
 * 
 * @returns The arctangent2 of the point (`x`, `y`)
 */

melM_wrapNumFunction2(atan2);

/***
 * @function exp
 * 
 * Returns e^`x` where e is the Euler number.
 * 
 * @arg x The exponent
 * @returns e^`x`
 */

melM_wrapNumFunction1(exp);

/***
 * @function log
 * 
 * Returns the natural logarithm of `x`.
 * 
 * @arg x The argument of the logarithm
 * @returns The natural logarithm of `x`
 */

melM_wrapNumFunction1(log);

/***
 * @function round
 * 
 * Return `x` rounded.
 * 
 * @arg x A `Number`
 * @returns An `Integer`
 */

melM_wrapNumFunction1Type(round, MELON_TYPE_INTEGER, integer);

/***
 * @function floor
 * 
 * Returns the largest integer less than or equal to `x`.
 * 
 * @arg x A `Number`
 * @returns An `Integer`
 */

melM_wrapNumFunction1Type(floor, MELON_TYPE_INTEGER, integer);

/***
 * @function ceil
 * 
 * Returns `x` rounded to the next largest integer.
 * 
 * @arg x A `Number`
 * @returns An `Integer`
 */

melM_wrapNumFunction1Type(ceil, MELON_TYPE_INTEGER, integer);

/***
 * @function abs
 * 
 * Returns the absolute value of `x`.
 * 
 * @arg x A `Number`
 * @returns The absolute value of `x`
 */

melM_wrapNumFunction1(fabs);

/***
 * @function pow
 * 
 * Performs `base`^`exp`.
 * This is equivalent to the [power operator](operators.md#arithmetic-operators) which is
 * faster and thus preferable when possibile.
 * 
 * @arg base The exponent's base
 * @arg exp The exponent
 * 
 * @returns base^exp
 */

melM_wrapNumFunction2(pow);

/***
 * @function isNaN
 * 
 * Checks whether a given `Number` is a `NaN` value.
 * 
 * @arg n The `Number` to check
 * @returns `true` if `n` is `NaN`, `false` otherwise.
 */

melM_wrapNumFunction1Type(isnan, MELON_TYPE_BOOL, boolean);

/***
 * @function isInfinity
 * 
 * Checks whether a given `Number` represents infinity.
 * 
 * @arg n The `Number` to check
 * @returns `true` if `n` represents infinity, `false` otherwise.
 */

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