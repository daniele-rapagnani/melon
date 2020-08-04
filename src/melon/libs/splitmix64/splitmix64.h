#ifndef __splitmix64_h__
#define __splitmix64_h__

#include <stdint.h>

extern uint64_t spmx64_seed;
uint64_t spmx64_next();

#endif // __splitmix64_h__