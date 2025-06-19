cmake_minimum_required(VERSION 3.10)

include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckCSourceCompiles)

# Default to disabled for all mutex/barrier/thread options
set(USE_PTHREAD_MUTEX 0)
set(USE_PTHREAD_SPINLOCK 0)
set(USE_OSX_SPINLOCK 0)
set(USE_OMP_LOCK 0)
set(USE_ATOMIC_SPINLOCK 0)

set(USE_PTHREAD_BARRIER 0)
set(USE_SPIN_BARRIER 0)

set(USE_PTHREADS_THREADS 0)
set(USE_OPENMP_THREADS 0)
set(USE_TBB_THREADS 0)
set(USE_PPL_THREADS 0)
set(USE_OMPTASK_THREADS 0)
set(USE_DISPATCH_THREADS 0)
set(USE_WINDOWS_THREADS 0)

# Detect pthread support
find_package(Threads)
if(Threads_FOUND)
    set(USE_PTHREADS_THREADS 1)
    set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    check_symbol_exists(pthread_mutex_init pthread.h HAVE_PTHREAD_MUTEX)
    if(HAVE_PTHREAD_MUTEX)
        set(USE_PTHREAD_MUTEX 1)
    endif()
    check_symbol_exists(pthread_spin_init pthread.h HAVE_PTHREAD_SPIN)
    if(HAVE_PTHREAD_SPIN)
        set(USE_PTHREAD_SPINLOCK 1)
    endif()
    check_symbol_exists(pthread_barrier_init pthread.h HAVE_PTHREAD_BARRIER)
    if(HAVE_PTHREAD_BARRIER)
        set(USE_PTHREAD_BARRIER 1)
    endif()
endif()

# Detect OpenMP
find_package(OpenMP)
if(OpenMP_C_FOUND)
    set(USE_OPENMP_THREADS 1)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    list(APPEND EXTRA_LIBS OpenMP::OpenMP_C)
endif()

# Detect libdispatch (Apple GCD)
check_include_file(dispatch/dispatch.h HAVE_DISPATCH_H)
find_library(DISPATCH_LIB dispatch)
if(HAVE_DISPATCH_H AND DISPATCH_LIB)
    set(USE_DISPATCH_THREADS 1)
    list(APPEND EXTRA_LIBS ${DISPATCH_LIB})
endif()

# Detect TBB
find_package(TBB QUIET)
if(TBB_FOUND)
    set(USE_TBB_THREADS 1)
    list(APPEND EXTRA_LIBS TBB::tbb)
endif()

# Windows or PPL (only if building with MSVC)
if(WIN32)
    set(USE_WINDOWS_THREADS 1)
endif()

# Verify availability of GCC style __atomic builtins
check_c_source_compiles(
"#include <stdint.h>\nint main(void){int a=0;int cmp=0;return __atomic_compare_exchange_n(&a,&cmp,1,0,5,5)?0:1;}" HAVE_GCC_ATOMICS)
if(NOT HAVE_GCC_ATOMICS)
    message(FATAL_ERROR "__atomic builtins required")
endif()

# Use atomic spinlock if nothing else
if(NOT USE_PTHREAD_MUTEX AND NOT USE_PTHREAD_SPINLOCK AND
   NOT USE_OSX_SPINLOCK AND NOT USE_OMP_LOCK)
    set(USE_ATOMIC_SPINLOCK 1)
endif()

# Use spin barrier if no pthread barrier
if(NOT USE_PTHREAD_BARRIER)
    set(USE_SPIN_BARRIER 1)
endif()

configure_file(tci/tci_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/tci/tci_config.h @ONLY)

add_library(tci
    tci/barrier.c
    tci/context.c
    tci/mutex.c
    tci/parallel.c
    tci/slot.c
    tci/work_item.c
    tci/communicator.c
    tci/task_set.c
)

target_include_directories(tci PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/tci
    ${CMAKE_CURRENT_BINARY_DIR}/tci
)

target_link_libraries(tci PUBLIC Threads::Threads ${EXTRA_LIBS})

