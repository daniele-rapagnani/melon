#ifndef __melon__closure_h__
#define __melon__closure_h__

#include "melon/core/types.h"
#include "melon/core/value.h"
#include "melon/core/gc_item.h"

#define melM_closureFromObj(obj) ((Closure*)((TPtr)(obj) + sizeof(GCItem)))
#define melM_closureUpvaluesFromObj(obj) ((Upvalue**)((TPtr)(obj) + sizeof(GCItem) + sizeof(Closure)))

struct Upvalue;
typedef struct Upvalue Upvalue;

typedef struct Closure
{
    GCItem* fn;
} Closure;

GCItem* melNewClosure(VM* vm, GCItem* fn);
TRet melFreeClosure(VM* vm, GCItem* item);

#endif // __melon__closure_h__