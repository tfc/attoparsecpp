add_executable(${PROJECT_NAME}-test 
  gdb.cpp
  ini.cpp
  main.cpp
  math_expression.cpp
  test.cpp
  )
target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})
target_compile_options(${PROJECT_NAME}-test PRIVATE -Wall -Wextra -Werror)
target_link_libraries(${PROJECT_NAME}-test Catch2::Catch2)

catch_discover_tests(${PROJECT_NAME}-test)
