#ifndef __melon__macros_h__
#define __melon__macros_h__

#define melM_min(a, b) ((a) < (b) ? (a) : (b))
#define melM_max(a, b) ((a) > (b) ? (a) : (b))
#define melM_vaargs(...) , ##__VA_ARGS__

#define melM_setBits(var, bits) \
    ((var) |= (bits))

#define melM_clearBits(var, bits) \
    ((var) &= (~(bits)))

#define melM_checkBits(var, bits) \
    (((var) & (bits)) == (bits))

#define melM_maxBitsValue(bits) \
    (((TUInteger)1 << (bits)) - 1)

#define melM_getBits(var, skip, count) \
    (((var) >> (skip)) & (melM_maxBitsValue(count)))

#define melM_stringify_helper(x) #x
#define melM_stringify(x) melM_stringify_helper(x)

#ifdef __GNUC__
#define likely(x)      __builtin_expect(!!(x), 1) 
#define unlikely(x)    __builtin_expect(!!(x), 0)
#else
#define likely(x)      do {} while(0) 
#define unlikely(x)    do {} while(0)
#endif

#endif // __melon__macros_h__