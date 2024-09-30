include(CTest)

find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME}-gtest
  math_expression.cpp
  )
target_link_libraries(${PROJECT_NAME}-gtest ${PROJECT_NAME} GTest::gtest_main)
target_compile_features(${PROJECT_NAME}-gtest INTERFACE cxx_std_17)
target_compile_options(${PROJECT_NAME}-gtest PRIVATE -Wall -Wextra -Werror)

catch_discover_tests(${PROJECT_NAME}-gtest)
