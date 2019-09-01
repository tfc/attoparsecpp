add_executable(${PROJECT_NAME}-benchmark main.cpp)
target_link_libraries(${PROJECT_NAME}-benchmark ${PROJECT_NAME})
target_compile_options(${PROJECT_NAME}-benchmark PRIVATE -O3 -Wall -Wextra -Werror)
target_link_libraries(${PROJECT_NAME}-benchmark benchmark::benchmark pthread)


