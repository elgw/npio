cmake_minimum_required(VERSION 3.9)

project(npio
  LANGUAGES C)

set (CMAKE_C_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")


add_library( npio src/npio.c src/dp.c )
set_target_properties(npio PROPERTIES PUBLIC_HEADER "include/npio.h")

target_include_directories(npio PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

INSTALL(TARGETS npio)
