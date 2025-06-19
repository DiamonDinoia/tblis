cmake_minimum_required(VERSION 3.10)
project(tblis LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Generate configuration headers for minimal build
set(INCLUDE_CONFIGS "#include \"configs/reference/config.hpp\"")
set(FOREACH_CONFIG "FOREACH_CONFIG(reference_config)")
set(FOREACH_CONFIG_AND_TYPE "#define FOREACH_TYPE(type) \\\nFOREACH_CONFIG_AND_TYPE(type, reference_config)\n#include \"foreach_type.h\"")
set(FOREACH_TYPE "FOREACH_TYPE(float)\nFOREACH_TYPE(double)\nFOREACH_TYPE(scomplex)\nFOREACH_TYPE(dcomplex)")
configure_file(src/configs/include_configs.hpp.in src/configs/include_configs.hpp @ONLY)
configure_file(src/configs/foreach_config.h.in src/configs/foreach_config.h @ONLY)
configure_file(src/configs/foreach_config_and_type.h.in src/configs/foreach_config_and_type.h @ONLY)
configure_file(src/configs/foreach_type.h.in src/configs/foreach_type.h @ONLY)

# Create a header directory that mimics the installed layout
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include)
file(CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/include/tblis SYMBOLIC)

add_subdirectory(src/external/tci)

file(GLOB_RECURSE TBLIS_SRC
    src/iface/*.cxx
    src/internal/*.cxx
    src/configs/configs.cxx
    src/configs/reference/*.cxx
    src/util/*.cxx
    src/kernels/*.cxx
    src/matrix/*.cxx
)

add_library(tblis ${TBLIS_SRC})

# include directories
target_include_directories(tblis PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/external/marray/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/external/stl_ext/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/external/catch>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/external/tci>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src/external/tci>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src/configs>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
)

target_link_libraries(tblis PUBLIC tci pthread atomic)

# Tests
enable_testing()
add_executable(tblis_example test/test_install.cxx)
target_include_directories(tblis_example PRIVATE
    ${CMAKE_BINARY_DIR}/include
)
target_link_libraries(tblis_example PRIVATE tblis)
add_test(NAME tblis_example COMMAND tblis_example)

# Build and register each test file as its own executable
file(GLOB TEST_FILES
    test/1t/*.cxx
    test/3m/*.cxx
    test/3t/*.cxx
)
set(TEST_SUPPORT test/test.cxx)
foreach(src ${TEST_FILES})
    get_filename_component(name_we ${src} NAME_WE)
    get_filename_component(dir ${src} DIRECTORY)
    get_filename_component(dir_name ${dir} NAME)
    set(test_name "${dir_name}_${name_we}")
    add_executable(${test_name} ${TEST_SUPPORT} ${src})
    target_include_directories(${test_name} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/external/catch
        ${CMAKE_CURRENT_SOURCE_DIR}/src/external/marray/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/external/stl_ext/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/external/tci
        ${CMAKE_CURRENT_BINARY_DIR}/src/external/tci
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}/src/configs
        ${CMAKE_BINARY_DIR}/include
    )
    target_link_libraries(${test_name} PRIVATE tblis)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

