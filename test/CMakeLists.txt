cmake_minimum_required(VERSION 3.5)

file(GLOB_RECURSE TEST_FILES "./*.cpp")

include_directories("include")
add_executable(run_tests ${TEST_FILES})
target_link_libraries (run_tests self-driving-sim pthread)
