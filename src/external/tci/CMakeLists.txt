cmake_minimum_required(VERSION 3.10)

set(USE_PTHREAD_MUTEX 1)
set(USE_PTHREAD_BARRIER 1)
set(USE_PTHREADS_THREADS 1)
set(USE_ATOMIC_SPINLOCK 0)
set(USE_OMP_LOCK 0)
set(USE_OPENMP_THREADS 0)
set(USE_WINDOWS_THREADS 0)
set(USE_TBB_THREADS 0)
set(USE_PPL_THREADS 0)
set(USE_OMPTASK_THREADS 0)
set(USE_DISPATCH_THREADS 0)
set(USE_OSX_SPINLOCK 0)
set(USE_PTHREAD_SPINLOCK 0)
set(USE_SPIN_BARRIER 0)

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

target_link_libraries(tci PUBLIC pthread)

