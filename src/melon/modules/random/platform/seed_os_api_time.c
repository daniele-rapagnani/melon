#include "melon/modules/random/seed_os_api.h"

#include <time.h>

void melGetRandomSeed(TUint64* seed)
{
    *seed = time(NULL);
}