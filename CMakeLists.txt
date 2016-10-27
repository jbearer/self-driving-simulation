cmake_minimum_required(VERSION 3.5)
project(SelfDrivingSim)
include(ExternalProject)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

set(LOG_LEVEL info)

ExternalProject_Add(logging
  PREFIX tpc/logging
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  INSTALL_COMMAND true # Header only so we skip installation
)
include_directories(tpc/logging/src/logging/include)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/config/config.h.in"
  "${PROJECT_BINARY_DIR}/lib/config.h"
  )
include_directories("${PROJECT_BINARY_DIR}/lib")

include_directories("${PROJECT_SOURCE_DIR}/include")

add_subdirectory(src)
add_subdirectory(test)

install(DIRECTORY DESTINATION ${PROJECT_BINARY_DIR}/logs)
