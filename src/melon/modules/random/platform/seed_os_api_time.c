#include "melon/modules/random/seed_os_api.h"

#include <time.h>

void melGetRandomSeed(TSize* seed)
{
    *seed = time(NULL);
}