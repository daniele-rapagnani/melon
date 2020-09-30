#include "melon/core/gc.h"
#include "melon/core/object.h"
#include "melon/core/array.h"
#include "melon/core/closure.h"
#include "melon/core/tstring.h"
#include "melon/core/symbol.h"
#include "melon/core/range.h"
#include "melon/core/program.h"
#include "melon/core/native_iterator.h"
#include "melon/core/value.h"
#include "melon/core/vm.h"
#include "melon/core/stack.h"
#include "melon/core/utils.h"
#include "melon/core/lists.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef MELON_GC_MIN_GREY_STACK_CAPACITY
#define MELON_GC_MIN_GREY_STACK_CAPACITY 1024
#endif

#ifndef MELON_GC_DEFAULT_UNIT_OF_WORK
#define MELON_GC_DEFAULT_UNIT_OF_WORK 150
#endif

#ifdef MELON_GC_STATS_ENABLED
#define melM_gcStat(gc) \
    ((gc)->stats[(gc)->currentStat])
#endif

#ifdef _DEBUG_GC
#define melM_gcLog(...) printf("[GC] " __VA_ARGS__); fflush(stdout)
#define melM_gcLogObj(vm, obj, ...) printf("[GC] " __VA_ARGS__); melPrintGCItemUtils(vm, obj)
#else
#define melM_gcLog(...) do { } while(0)
#define melM_gcLogObj(obj, ...) do { } while(0)
#endif
#ifdef MELON_GC_STATS_ENABLED
#define melM_gcStatsUpdate(block) \
    { \
        GCStats* stat = &melM_gcStat(gc); \
        block \
    } \
    do { } while(0)

#define melM_gcStatsInit(block) \
    { \
        melGetTimeHD(&gc->lastGcStart); \
        melM_gcStatsUpdate(block); \
    } \
    do { } while(0)
#else
#define melM_gcStatsUpdate(block) do { } while(0)
#define melM_gcStatsInit(block) do { } while(0)
#endif // MELON_GC_STATS_ENABLED

static TBool gcSavedPauseState;

#ifndef NDEBUG
static TSize gcSavedNesting;
#endif

static TRet melVisitMarkGreyGC(VM* vm, void* gc, GCItem* item, TSize depth);
static TRet melVisitGrowItemOldGC(VM* vm, void* gc, GCItem* item, TSize depth);
static TRet melAgeItemGC(VM* vm, GC* gc, GCItem* item);
static TRet melGrowNursery(GC* gc);
static TRet melPushNursery(GC* gc, GCItem* item);
static TRet melRemoveItemNureseryIdx(GC* gc, TSize idx);
static TRet melRemoveItemNursery(GC* gc, GCItem* item);
static TRet melFindItemNursery(GC* gc, GCItem* item, TSize* index);

#ifndef NDEBUG
static void dumpNursery(const char* label, GC* gc)
{
    for (TSize i = 0; i < gc->nurserySize; i++)
    {
        printf("[NURSERY] %s: idx = %d, item = %p, size = %lu\n", label, i + 1, gc->nursery[i], gc->nurserySize);
    }
}
#endif

TRet melInitGC(GC* gc)
{
    gc->usedBytes = 0;
    gc->rootsList = NULL;
    gc->whitesList = NULL;
    gc->whitesCount = 0;
    gc->blacksList = NULL;
    gc->blacksCount = 0;
    gc->rangeIteratorPoolCount = 0;
    gc->paused = 0;
    gc->nextTriggerSize = MELON_GC_TRIGGER_DELTA;
    gc->nextTriggerDelta = MELON_GC_TRIGGER_DELTA;
    gc->nextMajorTriggerPecent = MELON_GC_MAJOR_TRIGGER_PERCENT;
    gc->nextMajorTriggerSize = MELON_GC_MAJOR_MIN_TRIGGER_SIZE;
    gc->phase = MELON_GC_STOPPED_PHASE;
    gc->unitOfWorkProgress = 0;
    gc->whiteMarker = 1;
    gc->greyMarker = 2;
    gc->blackMarker = 3;
    gc->totalCollected = 0;
    gc->allocatedCount = 0;

    gc->nurseryCapacity = 0;
    gc->nurserySize = 0;
    gc->nursery = NULL;
    melGrowBuffer((void**)&gc->nursery, &gc->nurseryCapacity, sizeof(GCItem*), MELON_GC_NURSERY_SIZE);
    memset(gc->nursery, 0, sizeof(GCItem*) * gc->nurseryCapacity);
    
    gc->greyStackCapacity = MELON_GC_MIN_GREY_STACK_CAPACITY;
    gc->greyStack = malloc(sizeof(GCItem*) * gc->greyStackCapacity);
    assert(gc->greyStack != NULL);
    gc->greyStackSize = 0;

#ifdef MELON_GC_STATS_ENABLED
    gc->statsCycle = 0;

    if (melClearStatsGC(gc) != 0)
    {
        return 1;
    }

    memset(&gc->globalStats, 0, sizeof(GCGlobalStats));
    gc->globalStats.minPauseTime = (~gc->globalStats.minPauseTime);
    
    melGetTimeHD(&gc->initTime);
#endif

    return 0;
}

#ifdef MELON_GC_STATS_ENABLED
static void updateStatsTimeGC(GC* gc)
{
    GCStats* stat = &melM_gcStat(gc);
    static MelTimeHD now;
    melGetTimeHD(&now);
    TUint64 diff = melGetTimeDiffNs(&gc->lastGcStart, &now);
    stat->durationNs += diff;
}

static void updateGlobalStats(GC* gc)
{
    TSize total = ((gc->statsCycle - 1) * MELON_GC_STATS_SAMPLES_COUNT);

    for (TSize i = 0; i < gc->currentStat; i++)
    {
        GCStats* stats = &gc->stats[i];
     
        TSize curSample = total + i + 1;

        if (stats->durationNs > gc->globalStats.maxPauseTime)
        {
            gc->globalStats.maxPauseTime = stats->durationNs / (stats->sweepIterations + stats->markIterations);
        }

        if (stats->durationNs > 0 && stats->durationNs < gc->globalStats.minPauseTime)
        {
            gc->globalStats.minPauseTime = stats->durationNs;
        }

        gc->globalStats.totalMinorCollections += stats->minor;
        gc->globalStats.totalMajorCollections += !stats->minor;
        gc->globalStats.totalScanned += stats->scanned;
        gc->globalStats.totalFreed += stats->freed;
        gc->globalStats.totalReclaimed += (stats->usedMemory - stats->usedMemoryAfter);
        gc->globalStats.totalTimeInGC += stats->durationNs;
        gc->globalStats.totalSweepIterations += stats->sweepIterations;
        gc->globalStats.totalMarkIterations += stats->markIterations;
    }

    static MelTimeHD now;
    melGetTimeHD(&now);
    gc->globalStats.totalTimeRuntime = melGetTimeDiffNs(&gc->initTime, &now);
}

TRet melClearStatsGC(GC* gc)
{
    gc->statsCycle++;
    gc->currentStat = 0;
    memset(gc->stats, 0, sizeof(GCStats) * MELON_GC_STATS_SAMPLES_COUNT);

    return 0;
}

TRet melDumpStatsGC(GC* gc)
{
    for (TSize i = 0; i < gc->currentStat; i++)
    {
        GCStats* stat = &gc->stats[i];

        printf(
            "%5llu/%5llu) %s, um = %10llu, t = %07.3fms, mi = %10llu, si = %10llu, s = %6llu, f = %10llu (%07.3f%%), uma = %10llu\n", 
            i,
            gc->statsCycle,
            stat->minor ? "m" : "j",
            stat->usedMemory,
            stat->durationNs / 1000000.0f,
            stat->markIterations,
            stat->sweepIterations,
            stat->scanned,
            stat->freed,
            (stat->freed / (float)stat->scanned) * 100.0f,
            stat->usedMemoryAfter
        );
    }

    return 0;
}

TRet melDumpGlobalStatsGC(GC* gc)
{
    updateGlobalStats(gc);

#ifdef _PRINT_STATS_GC
    if (gc->currentStat < MELON_GC_STATS_SAMPLES_COUNT)
    {
        melDumpStatsGC(gc);
    }
#endif

    GCGlobalStats* gstats = &gc->globalStats;
    TSize collections = gstats->totalSamples;

    printf(
        "=== GC stats ===\n"
        "total collections = " MELON_PRINTF_SIZE "\n"
        "time collecting: %fs\n"
        "total time: %fs\n"
        "time percent collecting: %f%%\n"
        "total scanned: " MELON_PRINTF_SIZE "\n"
        "total freed: " MELON_PRINTF_SIZE "\n"
        "efficency = %f%%\n"
        "total minor collections: " MELON_PRINTF_SIZE "\n"
        "total major collections: " MELON_PRINTF_SIZE "\n"
        "total mark iterations: " MELON_PRINTF_SIZE "\n"
        "total sweep iterations: " MELON_PRINTF_SIZE "\n"
        "avg mark iterations per collection: %f\n"
        "avg sweep iterations per collection: %f\n"
        "max pause = %fms\nmin pause = %fms\navg pause time: %fms\navg time per free = %fms\n"
        "total reclaimed: " MELON_PRINTF_SIZE " bytes\n===\n", 
        collections,
        gstats->totalTimeInGC / 1000000000.0f,
        gstats->totalTimeRuntime / 1000000000.0f,
        ((float)gstats->totalTimeInGC / (float)gstats->totalTimeRuntime) * 100.0f,
        gstats->totalScanned,
        gstats->totalFreed,
        (gstats->totalFreed / (float)gstats->totalScanned) * 100.0f,
        gstats->totalMinorCollections,
        gstats->totalMajorCollections,
        gstats->totalMarkIterations,
        gstats->totalSweepIterations,
        gstats->totalMarkIterations / (float)collections,
        gstats->totalSweepIterations / (float)collections,
        gstats->maxPauseTime / 1000000.0f,
        gstats->minPauseTime / 1000000.0f,
        (gstats->totalTimeInGC / 1000000.0f) / (float)(gstats->totalMarkIterations + gstats->totalSweepIterations),
        (gstats->totalTimeInGC / 1000000.0f) / (float)gstats->totalFreed,
        gstats->totalReclaimed
    );

    return 0;
}

static void updateCycleStats(GC* gc)
{
    GCStats* stat = &melM_gcStat(gc);

    stat->usedMemoryAfter = gc->usedBytes;
    gc->currentStat++;
    gc->globalStats.totalSamples++;
    
    if (gc->currentStat >= MELON_GC_STATS_SAMPLES_COUNT)
    {
        updateGlobalStats(gc);

#ifdef _PRINT_STATS_GC
        melDumpStatsGC(gc);
#endif

        melClearStatsGC(gc);
    }
}

static void beginCycleStats(VM* vm, GC* gc)
{
    GCStats* stat = &melM_gcStat(gc);

    if (!melM_stackIsEmpty(&vm->callStack))
    {
        CallFrame* cf = melGetTopCallFrameVM(vm);

        if (cf->function->debug.count > 0)
        {
            stat->vmSourceLine = cf->function->debug.lines[cf->pc];

            if (cf->function->debug.file != NULL)
            {
                strncpy(&stat->vmSourceFile[0], cf->function->debug.file, MELON_GC_STATS_MAX_FILENAME - 1);
                stat->vmSourceFile[MELON_GC_STATS_MAX_FILENAME - 1] = '\0';
            }
        }
    }

    stat->usedMemory = gc->usedBytes;
}
#endif // MELON_GC_STATS_ENABLED

static void melStatsEndIteration(GC* gc)
{
#ifdef MELON_GC_STATS_ENABLED
    updateStatsTimeGC(gc);
#endif
}

static void melStatsStartCycle(VM* vm, GC* gc)
{
#ifdef MELON_GC_STATS_ENABLED
    beginCycleStats(vm, gc);
#endif
}

static void melStatsEndCycle(GC* gc)
{
    melStatsEndIteration(gc);
#ifdef MELON_GC_STATS_ENABLED
    updateCycleStats(gc);
#endif
}

TRet melIsPausedGC(VM* vm, GC* gc)
{
    return gc->paused ? 0 : 1;
}

TRet melSetPauseGC(VM* vm, GC* gc, TBool paused)
{
    gc->paused = paused;
    return 0;
}

TRet melSaveAndPauseGC(VM* vm, GC* gc)
{
    // Check that we are not nesting melSaveAndPausGC calls
    // otherwise we would need a stack
    assert(gcSavedNesting == 0);

    gcSavedPauseState = gc->paused;
#ifndef NDEBUG
    gcSavedNesting++;
#endif

    melSetPauseGC(vm, gc, 1);

    return 0;
}

TRet melRestorePauseGC(VM* vm, GC* gc)
{
#ifndef NDEBUG
    gcSavedNesting--;
#endif
    melSetPauseGC(vm, gc, gcSavedPauseState);

    return 0;
}

static TRet melProcessGreyStack(VM* vm, GC* gc, TSize unitOfWorkCap)
{
    melM_gcLog(" --- Processing grey stack\n");

    while(gc->greyStackSize > 0)
    {
        assert(gc->greyStackSize > 0);
        GCItem* item = gc->greyStack[--gc->greyStackSize];
        
        melMarkBlackGC(vm, gc, item);
        
        if (unitOfWorkCap > 0 && gc->unitOfWorkProgress >= unitOfWorkCap)
        {
            break;
        }
    }

    melM_gcLog(" ---- Processing grey stack ended\n");

    return 0;
}

static void melSwapColorMarkers(GC* gc)
{
    melM_gcLog("Swapping whites and blacks\n");

    TUint8 tmp = gc->whiteMarker;
    gc->whiteMarker = gc->blackMarker;
    gc->blackMarker = tmp;
    
    GCItem* tmpPtr = gc->whitesList;
    gc->whitesList = gc->blacksList;
    gc->blacksList = tmpPtr;

    TSize tmpSize = gc->whitesCount;
    gc->whitesCount = gc->blacksCount;
    gc->blacksCount = tmpSize;  
}

static void melEndMajorCycle(GC* gc)
{
    melM_gcLog(" ------ Major cycle ended\n");

    melStatsEndCycle(gc);
    melSwapColorMarkers(gc);
    
    float nextTriggerSize = gc->whitesCount * ((float)gc->nextMajorTriggerPecent / 100.0f);
    gc->nextMajorTriggerSize +=
        melM_max((TSize)roundf(nextTriggerSize), MELON_GC_MAJOR_MIN_TRIGGER_SIZE)
    ;
    
    gc->phase = MELON_GC_STOPPED_PHASE;
}

static void melMarkRoots(VM* vm, GC* gc)
{
    RootNodeGC* curRoot = gc->rootsList;
 
    melM_gcLog(" --- Marking roots grey\n");

    while (curRoot != NULL)
    {
        melMarkGreyGC(vm, gc, curRoot->item);
        curRoot = curRoot->next;
    }

    melM_gcLog(" --- Roots marked\n");
    melM_gcLog(" --- Marking stack grey\n");

#ifdef _DEBUG_GC
    melPrintStackUtils(vm);
#endif

    for (TSize i = 0; i < vm->stack.top; i++)
    {
        melMarkGreyValueGC(vm, gc, &vm->stack.stack[i]);
    }

    melM_gcLog(" --- Stack marked\n");
    melM_gcLog(" --- Marking call stack grey\n");

#ifdef _DEBUG_GC
    melPrintCallStackUtils(vm);
#endif

    for (TSize i = 0; i < vm->callStack.top; i++)
    {
        CallFrame* cf = melM_stackGet(&vm->callStack, i);
        melMarkGreyGC(vm, gc, cf->closure);
    }

    melM_gcLog(" --- Call stack marked\n");
    melM_gcLog(" --- Marking range iterator pool grey\n");

    for (TSize i = 0; i < gc->rangeIteratorPoolCount; i++)
    {
        melMarkGreyGC(vm, gc, gc->rangeIteratorPool[i]);
    }

    melM_gcLog(" --- Range iterator marked\n");
}

static void melDoMinorCollection(VM* vm, GC* gc)
{
    melM_gcLog(" ------- Starting minor phase " MELON_PRINTF_SIZE "\n", gc->globalStats.totalSamples);

    melStatsStartCycle(vm, gc);

    melM_gcStatsInit({
        stat->markIterations = 1;
        stat->minor = 1;
    });

    gc->phase = MELON_GC_MINOR_PHASE;

    melMarkRoots(vm, gc);
    melProcessGreyStack(vm, &vm->gc, 0);

    TSize idx;
    GCItem* it;

    for (TSize i = gc->nurserySize; i > 0; i--)
    {
        idx = i - 1;
        it = gc->nursery[idx];

        if (melM_isDarkGCItem(gc, it))
        {
            melM_gcLog("Skipping item in nursery because it aged: %p\n", it);
            it->gcState.color = gc->whiteMarker;
            continue;
        }

        melFreeItemGC(vm, gc, it);
        melRemoveItemNureseryIdx(gc, idx);

        melM_gcStatsUpdate({
            stat->scanned++;
            stat->freed++;
        });
    }

    melStatsEndCycle(gc);

    if (gc->whitesCount >= gc->nextMajorTriggerSize)
    {
        melM_gcLog("Processing " MELON_PRINTF_INT " items still left in the nursery\n", gc->nurserySize);

        // Some objects may be still in the nursery.
        // We need to promote them so that they can be
        // processed in the major collection.
        // We must do it here because items must be ready
        // before the next mark step: they may be processed
        // by barriers.

        while (gc->nurserySize > 0)
        {
            melVisitGrowItemOldGC(vm, (void*)gc, gc->nursery[gc->nurserySize - 1], 0);
        }

        melM_gcLog(
            " ------- Whites count %ld >= %ld: starting major collection\n",
            gc->whitesCount, 
            gc->nextMajorTriggerSize
        );

        gc->phase = MELON_GC_MARK_PHASE;
        melStatsStartCycle(vm, gc);
    }
    else
    {
        melM_gcLog(
            " ------- Minor collection terminated (whites count = %ld)\n", 
            gc->whitesCount
        );

        gc->phase = MELON_GC_STOPPED_PHASE;
        gc->nextTriggerSize = gc->allocatedCount + gc->nextTriggerDelta;
    }
}

static void melDoMarkIteration(VM* vm, GC* gc)
{
    melM_gcStatsInit({
        stat->minor = 0;
        stat->markIterations++;
    });

    assert(gc->nurserySize == 0);

    melMarkRoots(vm, gc);

    melM_gcLog(
        " ------- Marking step " MELON_PRINTF_INT ": grey = " MELON_PRINTF_INT ", white = " MELON_PRINTF_INT "\n",
        gc->globalStats.totalSamples, 
        gc->greyStackSize, 
        gc->whitesCount
    );

    melProcessGreyStack(
        vm, 
        gc, 
#ifndef _STRESS_GC
        MELON_GC_DEFAULT_UNIT_OF_WORK
#else
        0
#endif
    );

    if (gc->greyStackSize == 0)
    {
        gc->phase = MELON_GC_SWEEP_PHASE;

        melM_gcLog("Marking completed: white objects = " MELON_PRINTF_SIZE "\n", gc->whitesCount);

        if (gc->whitesCount == 0)
        {
            melM_gcLog("Nothing to sweep, stopping.\n");
            melEndMajorCycle(gc);
        }
#ifdef _STRESS_GC
        else
        {
            melTriggerGC(vm, gc);
        }
#endif
    }

    melStatsEndIteration(gc);
}

static void melDoSweepIteration(VM* vm, GC* gc)
{
    melM_gcStatsInit({
        stat->sweepIterations++;
    });

    GCItem* curItem = gc->whitesList;
    GCItem* prevItem = NULL;

    melM_gcLog(" ------- Sweeping " MELON_PRINTF_SIZE " white objects\n", gc->whitesCount);

    while (curItem != NULL)
    {
        prevItem = curItem->prev;
        
        gc->unitOfWorkProgress++;
        melFreeItemGC(vm, &vm->gc, curItem);

        melM_gcStatsUpdate({
            stat->scanned++;
            stat->freed++;
        });

#ifndef _STRESS_GC
        if (
            MELON_GC_DEFAULT_UNIT_OF_WORK > 0 && 
            gc->unitOfWorkProgress >= MELON_GC_DEFAULT_UNIT_OF_WORK
        )
        {
            melStatsEndIteration(gc);
            return;
        }
#endif // _STRESS_GC

        curItem = prevItem != NULL ? prevItem->next : gc->whitesList;
    }

    melEndMajorCycle(gc);
    melM_gcLog("Sweep done, used now: %lu bytes\n", gc->usedBytes);
}

TRet melTriggerGC(VM* vm, GC* gc)
{
#ifndef _STRESS_GC
    if (gc->phase == MELON_GC_STOPPED_PHASE && gc->allocatedCount < gc->nextTriggerSize)
    {
        return 0;
    }
#endif

    if (gc->paused != 0)
    {
        return 0;
    }

    melM_gcLog(" ------ Triggering GC: allocated " MELON_PRINTF_SIZE " bytes\n", gc->usedBytes);

#ifdef _DEBUG_GC
    melPrintVMCurrentLocation(vm);
    melPrintNativeStackUtils();
#endif

    if (gc->phase == MELON_GC_STOPPED_PHASE)
    {
        melDoMinorCollection(vm, gc);
        return 0;
    }

    gc->unitOfWorkProgress = 0;

    if (gc->phase == MELON_GC_MARK_PHASE)
    {
        melDoMarkIteration(vm, gc);
    }
    else
    {
        melDoSweepIteration(vm, gc);
    }

    return 0;
}

TRet melAddRootValueGC(VM* vm, GC* gc, Value* value)
{
    if (!melM_isGCItem(value))
    {
        return 1;
    }

    melAddRootGC(vm, gc, value->pack.obj);

    return 0;
}

TRet melAddRootGC(VM* vm, GC* gc, GCItem* item)
{
    RootNodeGC* node = malloc(sizeof(RootNodeGC));
    assert(node != NULL);

    gc->usedBytes += sizeof(RootNodeGC);

    node->item = item;
    node->next = gc->rootsList;
    gc->rootsList = node;

    return 0;
}

TRet melRemoveRootGC(VM* vm, GC* gc, GCItem* item)
{
    RootNodeGC* node = gc->rootsList;
    RootNodeGC* prevNode = NULL;

    while (node != NULL)
    {
        if (node->item == item)
        {
            if (prevNode == NULL)
            {
                gc->rootsList = node->next;
            }
            else
            {
                prevNode->next = node->next;
            }

#ifdef _ZERO_MEMORY_ON_FREE_GC
            memset(node, 0, sizeof(RootNodeGC));
#endif
            free(node);

            gc->usedBytes -= sizeof(RootNodeGC);

            return 0;
        }

        prevNode = node;
        node = node->next;
    }

    return 1;
}

TRet melMarkGreyGC(VM* vm, GC* gc, GCItem* item)
{
    assert(item->type > MELON_TYPE_NONE && item->type <= MELON_TYPE_MAX_ID);

    melM_gcLog(
        "Visiting %p for grey %s marking\n",
        item,
        gc->phase == MELON_GC_MARK_PHASE ? "major" : "minor"
    );

    // Don't follow young -> old references in minor collections
    if (gc->phase == MELON_GC_MINOR_PHASE && melM_isOldGCItem(gc, item))
    {
        melM_gcLog("Skipping %p grey marking, old item and minor collection.\n", item);
        return 0;
    }

    if (melM_isDarkGCItem(gc, item))
    {
        melM_gcLog("Skipping %p grey marking, item is already dark.\n", item);
        return 0;
    }

    melM_gcLogObj(
        vm, item,
        "Marking %s grey: %p\n", 
        gc->phase == MELON_GC_MARK_PHASE ? "major" : "minor", 
        item
    );

    melGrowBuffer((void**)&gc->greyStack, &gc->greyStackCapacity, sizeof(GCItem*), gc->greyStackSize + 1);

    melM_setColorGCItem(gc, item, gc->greyMarker);
    gc->greyStack[gc->greyStackSize++] = item;

    gc->unitOfWorkProgress++;

    if (gc->phase == MELON_GC_MARK_PHASE)
    {
        melRemoveWhiteGC(gc, item);
    }

    return 0;
}

TRet melAddBlackGC(GC* gc, GCItem* item)
{
    melM_gcLog("Adding %p to blacks list\n", item);
    melM_setColorGCItem(gc, item, gc->blackMarker);
    melM_shiftDLList(gc->blacksList, item, gc->blacksCount);
    return 0;
}

static TRet melVisitMarkGreyGC(VM* vm, void* vgc, GCItem* item, TSize depth)
{
    assert(vgc != NULL);
    GC* gc = (GC*)vgc;

    melMarkGreyGC(vm, gc, item);

    return depth;
}

TRet melMarkBlackGC(VM* vm, GC* gc, GCItem* item)
{
    if (melM_isBlackGCItem(gc, item))
    {
        return 1;
    }

    melM_gcLog("melMarkBlackGC(%p)\n", item);

#ifdef _TRACK_ALLOCATIONS_GC
    if (item == vm->global)
    {
        melM_gcLog("Marking Global object black\n");
    }
    else
    {
        melPrintGCItemUtils(vm, item);
    }
#endif

    melM_gcStatsUpdate({
        stat->scanned++;
    });

    if (gc->phase == MELON_GC_MINOR_PHASE)
    {
        if (!melM_isOldGCItem(gc, item))
        {
            melAgeItemGC(vm, gc, item);
        }
    }
    else
    {
        melAddBlackGC(gc, item);
    }

    melVisitGCItem(vm, item, melVisitMarkGreyGC, (void*)gc, 0);

    return 0;
}

TRet melMarkGreyValueGC(VM* vm, GC* gc, Value* item)
{    
    assert(item != NULL);
    assert(item->type > MELON_TYPE_NONE && item->type <= MELON_TYPE_MAX_ID);

    if (!melM_isGCItem(item))
    {
        return 0;
    }

    return melMarkGreyGC(vm, gc, item->pack.obj);
}

TRet melMarkBlackValueGC(VM* vm, GC* gc, Value* item)
{
    if (!melM_isGCItem(item))
    {
        return 0;
    }

    return melMarkBlackGC(vm, gc, item->pack.obj);
}

TRet melAddWhiteGC(GC* gc, GCItem* item)
{
    melM_gcLog("Adding %p to whites list\n", item);
    melM_setColorGCItem(gc, item, gc->whiteMarker);
    melM_shiftDLList(gc->whitesList, item, gc->whitesCount);
    return 0;
}

TRet melRemoveWhiteGC(GC* gc, GCItem* item)
{
    melM_gcLog("Removing %p from whites list\n", item);
    // melM_removeDLList(GCItem, gc->whitesList, item, gc->whitesCount);

    {                                                                              
        GCItem **prevNext = (item)->prev != NULL ? &(item)->prev->next : &(gc->whitesList);
                                                                                
        *prevNext = (item)->next;                                                  
                                                                                
        if ((item)->next != NULL)                                                  
        {                                                                          
            (item)->next->prev = (item)->prev;                                     
        }                                                                          
                                                                                
        assert((gc->whitesCount) > 0);                                                       
        (gc->whitesCount)--;                                                                 
    }
    return 0;
}

TRet melMarkWhiteGC(GC* gc, GCItem* item)
{
    if (melM_isBlackGCItem(gc, item))
    {
        melRemoveBlackGC(gc, item);
    }

    melAddWhiteGC(gc, item);
    return 0;
}

TRet melRemoveBlackGC(GC* gc, GCItem* item)
{
    melM_gcLog("Removing %p from blacks list\n", item);
    melM_removeDLList(GCItem, gc->blacksList, item, gc->blacksCount);
    return 0;
}

static TRet melGrowNursery(GC* gc)
{
    if (gc->nurseryCapacity <= gc->nurserySize)
    {
        TSize oldCapacity = gc->nurseryCapacity;
        melGrowBuffer((void**)&gc->nursery, &gc->nurseryCapacity, sizeof(GCItem*), gc->nurserySize);
        memset(&gc->nursery[oldCapacity], 0, sizeof(GCItem*) * (gc->nurseryCapacity - oldCapacity));
    }

    return 0;
}

static TRet melPushNursery(GC* gc, GCItem* item)
{
    gc->nurserySize++;

    if (melGrowNursery(gc) != 0)
    {
        return 1;
    }

    gc->nursery[gc->nurserySize - 1] = item;

    return 0;
}

static TRet melRemoveItemNureseryIdx(GC* gc, TSize idx)
{
    assert(gc->nurserySize > 0);
    gc->nurserySize--;

    if (idx == gc->nurserySize)
    {
        return 0;
    }

    memmove(
        &gc->nursery[idx],
        &gc->nursery[idx + 1],
        sizeof(GCItem*) * (gc->nurserySize - idx)
    );

    return 0;
}

static TRet melRemoveItemNursery(GC* gc, GCItem* item)
{
    TSize ni;
    
    if (melFindItemNursery(gc, item, &ni) != 0)
    {
        return 1;
    }

    return melRemoveItemNureseryIdx(gc, ni);
}

static TRet melFindItemNursery(GC* gc, GCItem* item, TSize* index)
{
    for (TSize ni = 0; ni < gc->nurserySize; ni++)
    {
        if (gc->nursery[ni] == item)
        {
            *index = ni;
            return 0;
        }
    }

    return 1;
}

TRet melAddNewbornItemGC(GC* gc, GCItem* item)
{
    melM_setColorGCItem(gc, item, gc->whiteMarker);
    melM_setAgeGCItem(gc, item, MELON_GC_INITIAL_AGE);

    if (melPushNursery(gc, item) != 0)
    {
        return 1;
    }

    return 0;
}

static TRet melVisitGrowItemOldGC(VM* vm, void* vgc, GCItem* item, TSize depth)
{
    assert(vgc != NULL);
    GC* gc = (GC*)vgc;

    if (melM_isOldGCItem(gc, item))
    {
        return 1;
    }
    
    melM_setAgeGCItem(gc, item, MELON_GC_OLD_AGE);

    if (melRemoveItemNursery(gc, item) != 0)
    {
        assert(0);
        return 0;
    }

    melAddWhiteGC(gc, item);

    return 0;
}

TRet melGrowItemOldGC(VM* vm, GC* gc, GCItem* item)
{
    return melVisitGCItem(vm, item, melVisitGrowItemOldGC, gc, 0);
}

static TRet melAgeItemGC(VM* vm, GC* gc, GCItem* item)
{
    assert(melM_getAgeGCItem(gc, item) <= MELON_GC_AGE_MAX);

    melM_setColorGCItem(gc, item, gc->blackMarker);
    TByte newAge = melM_getAgeGCItem(gc, item) + 1;
    melM_setAgeGCItem(gc, item, newAge);

    if (newAge == MELON_GC_AGE_MAX)
    {
        return melVisitGCItem(vm, item, melVisitGrowItemOldGC, gc, 0);
    }

    return 0;
}

TRet melPushRangeIteratorPool(GC* gc, GCItem* ni)
{
    if (gc->rangeIteratorPoolCount >= MELON_GC_RANGE_ITERATOR_POOL_SIZE)
    {
        return 1;
    }

    gc->rangeIteratorPool[gc->rangeIteratorPoolCount++] = ni;

    NativeIterator* niObj = melM_nativeitFromObj(ni);
    
    return 0;
}

TRet melPopRangeIteratorPool(GC* gc, const Value* value, GCItem** out)
{
    if (gc->rangeIteratorPoolCount == 0)
    {
        return 1;
    }

    *out = gc->rangeIteratorPool[--gc->rangeIteratorPoolCount];
    NativeIterator* ni = melM_nativeitFromObj(*out);
    ni->value = *value;

    return 0;
}

TRet melWriteBarrierGC(VM* vm, GCItem* obj, GCItem* value)
{
    if (melM_gcIsRunning(&vm->gc) && melM_isBlackGCItem(&vm->gc, obj))
    {
        melMarkGreyGC(vm, &vm->gc, value);
    }
    else if (
        !melM_gcIsMajorCollecting(&vm->gc)
        && melM_isOlderThanGCItem(&vm->gc, obj, value)
        && !melM_isOldGCItem(&gc->vm, value)
    )
    {
        melGrowItemOldGC(vm, &vm->gc, value);
    }

    return 0;
}

TRet melWriteBarrierValueGC(VM* vm, GCItem* obj, Value* value)
{
    if (melM_isGCItem(value))
    {
        melWriteBarrierGC(vm, obj, value->pack.obj);
    }

    return 0;
}

TRet melFreeItemGC(VM* vm, GC* gc, GCItem* item)
{
    if (!item)
    {
        return 0;
    }

#ifdef _TRACK_ALLOCATIONS_GC
    item->gcState.color = MELON_GC_STATE_FREED;
#endif

    gc->totalCollected++;

    switch(item->type)
    {
        case MELON_TYPE_OBJECT:
            return melFreeObject(vm, item);

        case MELON_TYPE_CLOSURE:
            return melFreeClosure(vm, item);

        case MELON_TYPE_ARRAY:
            return melFreeArray(vm, item);

        case MELON_TYPE_NATIVEIT:
            return melFreeNativeIterator(vm, item);

        case MELON_TYPE_STRING:
            return melFreeString(vm, item);

        case MELON_TYPE_PROGRAM:
            return melFreeProgram(vm, item);

        case MELON_TYPE_SYMBOL:
            return melFreeSymbol(vm, item);

        case MELON_TYPE_RANGE:
            return melFreeRange(vm, item);

        case MELON_TYPE_FUNCTION:
            return melFreeFunction(vm, item);

        default:
            assert(0);
    }

    return 1;
}

TRet melFreeValueGC(VM* vm, GC* gc, Value* item)
{
    if (!melM_isGCItem(item))
    {
        return 0;
    }

    return melFreeItemGC(vm, gc, item->pack.obj);
}