#include "melon/modules/random/random_module.h"
#include "melon/core/vm.h"
#include "melon/modules/modules.h"

#include "melon/libs/splitmix64/splitmix64.h"
#include "melon/libs/xoshiro256ss/xoshiro256starstar.h"
#include "melon/modules/random/seed_os_api.h"

/***
 * @module
 * 
 * This module exposes a PRNG and related functions to generate
 * random values from a seed.
 */ 

#include <stdlib.h>
#include <assert.h>

static const TUint16 MAX_SEED_ATTEMPTS = 1000;
static TSize globalSeed = 0;

static void setSeed(TSize seed)
{
    globalSeed = seed;
    spmx64_seed = seed;

    x256ss_state[0] = spmx64_next();
    x256ss_state[1] = spmx64_next();
    x256ss_state[2] = spmx64_next();
    x256ss_state[3] = spmx64_next();
}

static void ensureSeed(VM* vm)
{
    if (globalSeed != 0)
    {
        return;

    }

    TUint16 attempts = 0;
    TSize newSeed = 0;

    while (newSeed == 0)
    {
        if (attempts >= MAX_SEED_ATTEMPTS)
        {
            melM_fatal(vm, "Can't get valid seed, this is odd.");
            return;
        } 

        melGetRandomSeed(&newSeed);
        attempts++;
    }

    setSeed(newSeed);
}

/***
 * Sets the random seed.
 * 
 * @arg seed An `Integer` representing the new seed
 */

static TByte seedFunc(VM* vm)
{
    melM_arg(vm, seed, MELON_TYPE_INTEGER, 0);
    setSeed(seed->pack.value.integer);
    return 0;
}

/***
 * Gets the current seed value.
 * 
 * @returns An `Integer` representing the current seed
 */

static TByte getSeedFunc(VM* vm)
{
    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* result = melM_stackAllocRaw(&vm->stack);
    result->type = globalSeed != 0 ? MELON_TYPE_INTEGER : MELON_TYPE_NULL;
    result->pack.value.integer = globalSeed;

    return 1;
}

/***
 * Generates a random `Number` value, the resulting value will be
 * between `min` and `max` inclusive.
 * 
 * @arg ?min The lower bound, defaults to 0.0
 * @arg ?max The upper bound, defaults to 1.0
 * 
 * @returns A random `Number` between `min` and `max` inclusive.
 */

static TByte getRandomNumberFunc(VM* vm)
{
    ensureSeed(vm);

    melM_argOptional(vm, start, MELON_TYPE_NUMBER, 0);
    melM_argOptional(vm, end, MELON_TYPE_NUMBER, 1);

    if (start->type != end->type)
    {
        melM_fatal(vm, "You should specify both start and end when requesting a random range");
        return 0;
    }

    float startN = start->type != MELON_TYPE_NULL ? start->pack.value.number : 0.0f;
    float endN = start->type != MELON_TYPE_NULL ? end->pack.value.number : 1.0f;

    if (startN > endN)
    {
        float tmp = endN;
        endN = startN;
        startN = endN;
    }

    TUint64 val = x256ss_next();
    TNumber fval = (val >> 11) * 0x1.0p-53;
    fval *= (endN - startN);
    fval += startN;

    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* result = melM_stackAllocRaw(&vm->stack);
    result->type = MELON_TYPE_NUMBER;
    result->pack.value.number = fval;
    
    return 1;
}

/***
 * Generates a random `Integer` value, the resulting value will be
 * between `min` and `max` inclusive.
 * 
 * @arg ?min The lower bound, defaults to the minimum negative `Integer` representable
 * @arg ?max The upper bound, defaults to the maximum positive `Integer` representable
 * 
 * @returns A random `Integer` between `min` and `max` inclusive.
 */

static TByte getRandomIntFunc(VM* vm)
{
    ensureSeed(vm);

    melM_argOptional(vm, start, MELON_TYPE_INTEGER, 0);
    melM_argOptional(vm, end, MELON_TYPE_INTEGER, 1);

    if (start->type != end->type)
    {
        melM_fatal(vm, "You should specify both start and end when requesting a random range");
        return 0;
    }

    TInteger startN = start->type != MELON_TYPE_NULL ? start->pack.value.integer : MELON_INT_MIN;
    TInteger endN = start->type != MELON_TYPE_NULL ? end->pack.value.integer : MELON_INT_MAX;

    if (startN > endN)
    {
        TInteger tmp = endN;
        endN = startN;
        startN = endN;
    }

    TUint64 tval = x256ss_next() % (endN - startN);
    TInteger val = tval + startN;

    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* result = melM_stackAllocRaw(&vm->stack);
    result->type = MELON_TYPE_INTEGER;
    result->pack.value.integer = val;

    return 1;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "seed", 1, 0, &seedFunc },
    { "getSeed", 0, 0, &getSeedFunc },
    { "getRandomNumber", 2, 0, &getRandomNumberFunc },
    { "getRandomInt", 2, 0, &getRandomIntFunc },
    { NULL, 0, 0, NULL }
};

TRet melRandomModuleInit(VM* vm)
{
    return melNewModule(vm, funcs);
}