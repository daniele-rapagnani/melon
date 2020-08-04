#ifndef __xoshiro256starstar_h__
#define __xoshiro256starstar_h__

#include <stdint.h>

extern uint64_t x256ss_state[4];

uint64_t x256ss_next(void);
void x256ss_jump(void);
void x256ss_long_jump(void);

#endif // __xoshiro256starstar_h__