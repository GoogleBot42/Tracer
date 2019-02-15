cmake_minimum_required(VERSION 3.4.3)

project(RayTracer)

### config
set(source_name "tracer")
set(source_directory "src")
set(test_directory "test")
# build options
option(COMPUTECPP_SDK_USE_OPENMP "Enable OpenMP support" ON)
option(BUILD_TESTS "Build all tests." ON)
# build flags
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")

set(source_main ${source_directory}/main.cpp)

### find packages/deps
# ComputeCpp
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
find_package(ComputeCpp REQUIRED)

### create library
# Add project sources
file(GLOB_RECURSE lib_files ${source_directory}/*.cpp ${source_directory}/*.h ${source_directory}/*.hpp)
list(REMOVE_ITEM lib_files ${source_main})
# create the library
add_library(${source_name}lib ${lib_files})
# Add ComputeCpp for GPU computing
add_sycl_to_target(
  TARGET ${source_name}lib
  SOURCES ${lib_files}
)

### create main exe
add_executable(${source_name} ${source_main})
target_link_libraries(${source_name} ${source_name}lib ${GTKMM_LIBRARIES})

### create tests
if(BUILD_TESTS)
    include(GoogleTest)
    file(GLOB_RECURSE test_source_files ${test_directory}/*.cpp ${test_directory}/*.h)
    include_directories(${source_directory})
    add_executable(${source_name}_test ${test_source_files})
    gtest_add_tests(TARGET ${source_name}_test)
    target_link_libraries(${source_name}_test gtest gtest_main ${source_name}lib)
endif(BUILD_TESTS)
