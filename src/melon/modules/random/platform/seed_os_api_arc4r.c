#include "melon/modules/random/seed_os_api.h"

#include <stdlib.h>

void melGetRandomSeed(TSize* seed)
{
    arc4random_buf((void*)seed, sizeof(TSize));
}