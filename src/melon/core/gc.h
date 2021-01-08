#ifndef __melon__gc_h__
#define __melon__gc_h__

#include "melon/core/config.h"
#include "melon/core/types.h"
#include "melon/core/gc_item.h"
#include "melon/core/utils.h"

struct Value;
typedef struct Value Value;

#define MELON_GC_INITIAL_AGE 0
#define MELON_GC_OLD_AGE (MELON_GC_AGE_MAX + 1)

#if (defined(_PRINT_STATS_GC) || defined(_PRINT_GSTATS_GC))
#define MELON_GC_STATS_ENABLED
#endif

#define melM_gcIsRunning(gc) \
    ((gc)->phase != MELON_GC_STOPPED_PHASE)

#define melM_gcIsMajorCollecting(gc) \
    ((gc)->phase == MELON_GC_MARK_PHASE || (gc)->phase == MELON_GC_SWEEP_PHASE)

typedef struct RootNodeGC 
{
    GCItem* item;
    struct RootNodeGC* next;
} RootNodeGC;

typedef enum {
    MELON_GC_NONE_PHASE = 0,
    MELON_GC_STOPPED_PHASE,
    MELON_GC_MINOR_PHASE,
    MELON_GC_MARK_PHASE,
    MELON_GC_SWEEP_PHASE
} GCPhase;

#ifdef MELON_GC_STATS_ENABLED

typedef struct GCStats
{
    TBool minor;
    TSize usedMemory;
    TSize usedMemoryAfter;
    TSize markIterations;
    TSize sweepIterations;
    TUint64 durationNs;
    TSize scanned;
    TSize freed;
    TSize vmSourceLine;
    char vmSourceFile[MELON_GC_STATS_MAX_FILENAME];
} GCStats;

typedef struct GCGlobalStats
{
    TSize maxPauseTime;
    TSize minPauseTime;
    TSize totalReclaimed;
    TSize totalScanned;
    TSize totalFreed;
    TSize totalMinorCollections;
    TSize totalMajorCollections;
    TSize totalMarkIterations;
    TSize totalSweepIterations;
    TUint64 totalTimeInGC;
    TUint64 totalTimeRuntime;
    TSize totalSamples;
} GCGlobalStats;

#endif

typedef struct GC
{
    TSize usedBytes;
    RootNodeGC* rootsList;

    GCItem* rangeIteratorPool[MELON_GC_RANGE_ITERATOR_POOL_SIZE];
    TSize rangeIteratorPoolCount;

    GCItem* whitesList;
    TSize whitesCount;

    GCItem* blacksList;
    TSize blacksCount;

    GCItem** nursery;
    TSize nurserySize;
    TSize nurseryCapacity;
    
    TSize allocatedCount;
    TSize totalCollected;

    GCItem** greyStack;
    TSize greyStackSize;
    TSize greyStackCapacity;

    TSize nextTriggerSize;
    TSize nextTriggerDelta;
    TSize nextMajorTriggerSize;
    TSize nextMajorTriggerPecent;

    TUint8 blackMarker;
    TUint8 whiteMarker;
    TUint8 greyMarker;

    TSize unitOfWorkProgress;
    GCPhase phase;
    TBool paused;

#ifdef MELON_GC_STATS_ENABLED
    GCStats stats[MELON_GC_STATS_SAMPLES_COUNT];
    TUint16 currentStat;
    MelTimeHD initTime;
    MelTimeHD lastGcStart;
    TSize statsCycle;

    GCGlobalStats globalStats;
#endif
} GC;

TRet melInitGC(GC* gc);

#ifdef MELON_GC_STATS_ENABLED
TRet melClearStatsGC(GC* gc);
TRet melDumpStatsGC(GC* gc);
TRet melDumpGlobalStatsGC(GC* gc);
#endif

TRet melTriggerGC(VM* vm, GC* gc);

TRet melIsPausedGC(VM* vm, GC* gc);
TRet melSetPauseGC(VM* vm, GC* gc, TBool paused);
TRet melSaveAndPauseGC(VM* vm, GC* gc);
TRet melRestorePauseGC(VM* vm, GC* gc);

TRet melAddRootGC(VM* vm, GC* gc, GCItem* item);
TRet melAddRootValueGC(VM* vm, GC* gc, Value* value);
TRet melRemoveRootGC(VM* vm, GC* gc, GCItem* item);

TRet melMarkGreyGC(VM* vm, GC* gc, GCItem* item);
TRet melMarkGreyValueGC(VM* vm, GC* gc, Value* item);

TRet melAddWhiteGC(GC* gc, GCItem* item);
TRet melRemoveWhiteGC(GC* gc, GCItem* item);
TRet melMarkWhiteGC(GC* gc, GCItem* item);

TRet melMarkBlackGC(VM* vm, GC* gc, GCItem* item);
TRet melMarkBlackValueGC(VM* vm, GC* gc, Value* item);
TRet melAddBlackGC(GC* gc, GCItem* item);
TRet melRemoveBlackGC(GC* gc, GCItem* item);

TRet melAddNewbornItemGC(GC* gc, GCItem* item);
TRet melGrowItemOldGC(VM* vm, GC* gc, GCItem* item);

TRet melPushRangeIteratorPool(GC* gc, GCItem* ni);
TRet melPopRangeIteratorPool(GC* gc, const Value* value, GCItem** out);

TRet melWriteBarrierGC(VM* vm, GCItem* obj, GCItem* value);
TRet melWriteBarrierValueGC(VM* vm, GCItem* obj, Value* value);

TRet melFreeItemGC(VM* vm, GC* gc, GCItem* item);
TRet melFreeValueGC(VM* vm, GC* gc, Value* item);

#endif // __melon__gc_h__