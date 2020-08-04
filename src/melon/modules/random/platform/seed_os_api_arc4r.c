#include "melon/modules/random/seed_os_api.h"

#include <stdlib.h>

void melGetRandomSeed(TUint64* seed)
{
    arc4random_buf((void*)seed, sizeof(TUint64));
}