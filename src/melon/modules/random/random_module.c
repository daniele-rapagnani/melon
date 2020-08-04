#include "melon/modules/random/random_module.h"
#include "melon/core/vm.h"
#include "melon/modules/modules.h"

#include "melon/libs/splitmix64/splitmix64.h"
#include "melon/libs/xoshiro256ss/xoshiro256starstar.h"
#include "melon/modules/random/seed_os_api.h"

#include <stdlib.h>
#include <assert.h>

static const TUint16 MAX_SEED_ATTEMPTS = 1000;
static TUint64 globalSeed = 0;

static void setSeed(TUint64 seed)
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
    TUint64 newSeed = 0;

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

static TByte seedFunc(VM* vm)
{
    melM_arg(vm, seed, MELON_TYPE_INTEGER, 0);
    setSeed(seed->pack.value.integer);
    return 0;
}

static TByte getSeedFunc(VM* vm)
{
    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* result = melM_stackAllocRaw(&vm->stack);
    result->type = globalSeed != 0 ? MELON_TYPE_INTEGER : MELON_TYPE_NULL;
    result->pack.value.integer = globalSeed;

    return 1;
}

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

TByte getRandomIntFunc(VM* vm)
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