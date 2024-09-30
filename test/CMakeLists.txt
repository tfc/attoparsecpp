include(CTest)

find_package(Catch2 REQUIRED)
include(Catch)

add_executable(${PROJECT_NAME}-test
  gdb.cpp
  math_expression.cpp
  test.cpp
  )
target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})
target_compile_features(${PROJECT_NAME}-test INTERFACE cxx_std_17)
target_compile_options(${PROJECT_NAME}-test
  PRIVATE -Wall -Wextra -Werror)
target_link_libraries(${PROJECT_NAME}-test Catch2::Catch2WithMain)

catch_discover_tests(${PROJECT_NAME}-test)

if(CMAKE_BUILD_TYPE STREQUAL "Coverage")

  set(CMAKE_CXX_FLAGS_COVERAGE "-g -O0 --coverage")
  if(CMAKE_CXX_COMPILER STREQUAL "GNU")
    string(APPEND CMAKE_CXX_FLAGS_COVERAGE " -fprofile-abs-path")
  endif()
  set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "--coverage")
  set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE "--coverage")
  set(CMAKE_MODULE_LINKER_FLAGS_COVERAGE "--coverage")

  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    find_program(LLVM_COV_EXECUTABLE llvm-cov REQUIRED)
    file(CREATE_LINK
      ${LLVM_COV_EXECUTABLE} ${CMAKE_BINARY_DIR}/gcov SYMBOLIC)
    set(GCOV_EXECUTABLE "${LLVM_COV_EXECUTABLE} gcov")
  else()
    find_program(GCOV_EXECUTABLE gcov REQUIRED)
  endif()

  find_program(GCOVR_EXECUTABLE gcovr REQUIRED)
  configure_file(gcovr.cfg.in ${CMAKE_BINARY_DIR}/gcovr.cfg @ONLY)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/coverage)

  add_custom_target(process_coverage
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running gcovr to process coverage results"
    COMMAND ${GCOVR_EXECUTABLE} --config gcovr.cfg .
  )

endif()
