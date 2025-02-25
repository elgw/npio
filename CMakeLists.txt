cmake_minimum_required(VERSION 3.9)

# TODO: Default should be to build a static library, that makes most sense
# when including as a submodule
# Might be of interest:
# cmake -DCMAKE_BUILD_TYPE=Debug ..

project(npio
  VERSION 0.0.10
  LANGUAGES C)

set (CMAKE_C_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Don't install to /usr/local/ use /usr/
set( CMAKE_INSTALL_PREFIX
  /usr )

add_library( npio_shared SHARED src/npio.c )
set_property(TARGET npio_shared PROPERTY OUTPUT_NAME npio)

set_target_properties(npio_shared PROPERTIES PUBLIC_HEADER "src/npio.h")
set_target_properties(npio_shared PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION})

# Replace variables by their values
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/npio_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/npio_config.h)

target_include_directories(npio_shared PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable( npio_cli src/npio_cli.c src/npio.c )
set_property(TARGET npio_cli PROPERTY OUTPUT_NAME npio)

# Note the value of CMAKE_INSTALL_PREFIX
include(GNUInstallDirs)
INSTALL(TARGETS npio_shared
  LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR})
