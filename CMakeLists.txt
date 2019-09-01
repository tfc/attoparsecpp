cmake_minimum_required(VERSION 3.12)
project(attoparsecpp CXX)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

find_package(Catch2 REQUIRED)
find_package(benchmark REQUIRED)
include(CTest)
include(Catch)

enable_testing()
add_subdirectory(test)
add_subdirectory(benchmark)




