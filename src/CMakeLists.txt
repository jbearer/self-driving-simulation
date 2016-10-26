cmake_minimum_required(VERSION 3.5)

file(GLOB_RECURSE LIB_SRC "./*.cpp")

add_library(self-driving-sim ${LIB_SRC})

