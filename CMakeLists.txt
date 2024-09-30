cmake_minimum_required(VERSION 3.21)
project(attoparsecpp CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  find_program(CLANG_TIDY_EXECUTABLE clang-tidy REQUIRED)
  set(CMAKE_CXX_CLANG_TIDY
    ${CLANG_TIDY_EXECUTABLE} -p ${CMAKE_BINARY_DIR})
endif()

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

enable_testing()
add_subdirectory(test)
add_subdirectory(gtest)
add_subdirectory(benchmark)


set(CPACK_PACKAGE_NAME "attoparsecpp")
set(CPACK_PACKAGE_VENDOR "galowicz.de")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ Parser Library")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "attoparsecpp")
set(CPACK_PACKAGE_CONTACT "Jacek Galowicz <jacek@galowicz.de>")

set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_SET_DESTDIR true)

set(PKG_CONFIG_REQUIRES)
set(PKG_CONFIG_LIBDIR "\${prefix}/lib")
set(PKG_CONFIG_INCLUDEDIR "\${prefix}/include")
set(PKG_CONFIG_LIBS)
set(PKG_CONFIG_CFLAGS "-I\${includedir}")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/pkg-config.pc.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
  )

include(CPack)

include(GNUInstallDirs)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}-config)

install(
  EXPORT ${PROJECT_NAME}-config
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  )

install(
  DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/
  )

INSTALL(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
  )
