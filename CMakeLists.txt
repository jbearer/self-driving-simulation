cmake_minimum_required(VERSION 3.5)

project(SelfDrivingSim)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

set(LOG_LEVEL info)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/config/config.h.in"
  "${PROJECT_BINARY_DIR}/lib/config.h"
  )
include_directories("${PROJECT_BINARY_DIR}/lib")

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/tpc/logging/include"
)

add_subdirectory(src)
add_subdirectory(test)

install(DIRECTORY DESTINATION ${PROJECT_BINARY_DIR}/logs)
