cmake_minimum_required(VERSION 3.7)
project(MelonScriptVM)

option(DEBUG_GC "Build with garbage collection debugging on" OFF)
option(STRESS_GC "Build with garbage collection stress tests" OFF)
option(GSTATS_GC "Print global stats for the garbage collector when main function returns" OFF)
option(STATS_GC "Print stats for the garbage collector for every collection cycle" OFF)
option(DEBUG_COMPILER "Outputs compiled instructions as the compiler runs" OFF)
option(PROFILING_BUILD "Creates a build suitable for profiling" OFF)
option(BUILD_32 "Sets the build target to 32 bits" OFF)

if(DEBUG_GC)
    set(_DEBUG_GC ON)
    set(_TRACK_ALLOCATIONS_GC ON)
endif(DEBUG_GC)

if(DEBUG_GC OR GSTATS_GC)
    set(_PRINT_GSTATS_GC ON)
endif(DEBUG_GC OR GSTATS_GC)

if(DEBUG_GC OR STATS_GC)
    set(_PRINT_STATS_GC ON)
endif(DEBUG_GC OR STATS_GC)

if(STRESS_GC)
    set(_STRESS_GC ON)
    set(_ZERO_MEMORY_ON_FREE_GC ON)
endif(STRESS_GC)

if(DEBUG_COMPILER)
    set(_DEBUG_COMPILER ON)
endif(DEBUG_COMPILER)

if (APPLE)
    set(CMAKE_MACOSX_RPATH 0)
    set(ENABLE_CUSTOM_COMPILER_FLAGS Off)
endif (APPLE)

if ((UNIX OR MINGW) AND BUILD_32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
endif()

if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif(EMSCRIPTEN)

option(CJSON_OVERRIDE_BUILD_SHARED_LIBS "" On)
option(CJSON_BUILD_SHARED_LIBS "" Off)
option(ENABLE_CJSON_TEST "" Off)

include(CheckIncludeFile)
include(CheckSymbolExists)

check_include_file("sys/stat.h" HAVE_POSIX_STAT)
check_include_file("stdio.h" HAVE_STDIO)
check_include_file("unistd.h" HAVE_UNISTD)
check_include_file("libgen.h" HAVE_LIBGEN)
check_include_file("windows.h" HAVE_WINDOWS)
check_include_file("execinfo.h" HAVE_EXECINFO)

check_symbol_exists(arc4random_buf "stdlib.h" HAVE_ARC4RAND)

add_definitions(-Werror=incompatible-pointer-types)
add_definitions(-Werror=implicit-function-declaration)
add_definitions(-Werror=return-type)
add_definitions(-Werror=int-conversion)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_definitions(-Werror=macro-redefined)
endif()
add_definitions(-Wdouble-promotion)
add_definitions(-Wno-format)

set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

if (PROFILING_BUILD)
    add_definitions(-g)
    #add_definitions(-fno-inline-functions)
endif (PROFILING_BUILD)

if (HAVE_STDIO)
    message(STATUS "Using stdio API for the IO module")
    set(PLATFORM_SRCS ${PLATFORM_SRCS} src/melon/modules/io/platform/io_os_api_stdio.c)
endif(HAVE_STDIO)

if (HAVE_UNISTD AND HAVE_POSIX_STAT)
    message(STATUS "Using POSIX API for the FS module")
    set(PLATFORM_SRCS ${PLATFORM_SRCS} src/melon/modules/fs/platform/fs_os_api_posix.c)
endif(HAVE_UNISTD AND HAVE_POSIX_STAT)

if(HAVE_WINDOWS)
    message(STATUS "Using Windows API for the path module")
    set(PLATFORM_SRCS ${PLATFORM_SRCS} src/melon/modules/path/platform/path_os_api_windows.c)
elseif (HAVE_LIBGEN)
    message(STATUS "Using POSIX API for the path module")
    set(PLATFORM_SRCS ${PLATFORM_SRCS} src/melon/modules/path/platform/path_os_api_libgen.c)
endif()

if (HAVE_ARC4RAND OR HAVE_BSD_ARC4RAND)
    message(STATUS "Using BSD API for the random's seed module")
    set(PLATFORM_SRCS ${PLATFORM_SRCS} src/melon/modules/random/platform/seed_os_api_arc4r.c)
else()
    set(PLATFORM_SRCS ${PLATFORM_SRCS} src/melon/modules/random/platform/seed_os_api_time.c)
endif(HAVE_ARC4RAND OR HAVE_BSD_ARC4RAND)

configure_file(src/melon/core/build_config.h.in build_config.h)

add_library(
    melonscript-vm STATIC 
    src/melon/core/stack_meta.c
    src/melon/core/value.c
    src/melon/core/tstring.c
    src/melon/core/program.c
    src/melon/core/buffer.c
    src/melon/core/gc_item.c
    src/melon/core/gc.c
    src/melon/core/object.c
    src/melon/core/array.c
    src/melon/core/symbol.c
    src/melon/core/range.c
    src/melon/core/native_iterator.c
    src/melon/core/function.c
    src/melon/core/vm.c
    src/melon/core/lexer.c
    src/melon/core/compiler.c
    src/melon/core/serializer.c
    src/melon/core/deserializer.c
    src/melon/core/closure.c
    src/melon/core/utils.c
    src/melon/core/memory_utils.c
    src/melon/core/strings_table_openaddr.c
    src/melon/core/strings_table_chaining.c
    src/melon/core/opcodes.c
    
    src/melon/modules/modules.c
    src/melon/modules/io/io_module.c
    src/melon/modules/fs/fs_module.c
    src/melon/modules/path/path_module.c
    src/melon/modules/import/import_module.c
    src/melon/modules/string/string_module.c
    src/melon/modules/string/format.c
    src/melon/modules/object/object_module.c
    src/melon/modules/json/json_module.c
    src/melon/modules/math/math_module.c
    src/melon/modules/types/types_module.c
    src/melon/modules/compiler/compiler_module.c
    src/melon/modules/debug/debug_module.c
    src/melon/modules/cli/cli_module.c
    src/melon/modules/number/number_module.c
    src/melon/modules/integer/integer_module.c
    src/melon/modules/array/array_module.c
    src/melon/modules/function/function_module.c
    src/melon/modules/gc/gc_module.c
    src/melon/modules/random/random_module.c
    src/melon/modules/system/system_module.c

    ${PLATFORM_SRCS}
)

if (MINGW OR EMSCRIPTEN)
    message(STATUS "Disabling stack protector as it is not supported on MinGW and Emscripten")
    add_definitions(-fno-stack-protector)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fpermissive")
endif()

add_subdirectory(src/melon/libs/cJSON)

# Ignores the -Werror in cJSON that 
# causes issue with NAN casting in clang
target_compile_options(cjson PRIVATE -Wno-error)

add_library(
    external STATIC
    src/melon/libs/csiphash/csiphash.c
    src/melon/libs/xoshiro256ss/xoshiro256starstar.c
    src/melon/libs/splitmix64/splitmix64.c
)

target_include_directories(
    melonscript-vm PUBLIC     
    src/
    ${PROJECT_BINARY_DIR}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")

add_executable(
    assembler
    src/melon/tools/assembler.cpp
)

add_executable(
    melon
    src/melon/tools/melon.cpp
    src/melon/tools/utils.cpp
)

add_executable(
    disassembler
    src/melon/tools/disassembler.cpp
    src/melon/tools/utils.cpp
)

set(LINKLIBS melonscript-vm external cjson)

target_link_libraries(assembler ${LINKLIBS})
target_link_libraries(melon ${LINKLIBS})
target_link_libraries(disassembler ${LINKLIBS})

if (ANDROID)
	set(LINKLIBS ${LINKLIBS} log)
endif(ANDROID)

add_executable(
    tests
    tests/main.cpp
)

if (MINGW)
    # On MinGW add static linking of system libraries so that the resulting
    # executable is portable.
    # Also use lld to link the binaries: this solves some issues with catch2
    # and also speeds the build up considerably.
    
    set(MINGW_LD_FLAGS "-static-libgcc" "-static-libstdc++" "-static" "-lpthread" "-fuse-ld=lld")
    target_link_options(melon PUBLIC ${MINGW_LD_FLAGS})
    target_link_options(assembler PUBLIC ${MINGW_LD_FLAGS})
    target_link_options(disassembler PUBLIC ${MINGW_LD_FLAGS})
    target_link_options(tests PUBLIC ${MINGW_LD_FLAGS})
endif(MINGW)

if (EMSCRIPTEN)
    target_link_options(tests PUBLIC --embed-file ${CMAKE_SOURCE_DIR}/tests@/)
endif (EMSCRIPTEN)

target_link_libraries(tests ${LINKLIBS})
