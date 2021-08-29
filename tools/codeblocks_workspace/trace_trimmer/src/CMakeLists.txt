cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_ECLIPSE_VERSION "4.14.0" CACHE STRING "4.14.0")

# set the project name
project(trace_trimmer)


add_compile_options("-Wall" "-Wpedantic" "-Wextra" "-fexceptions" "-Werror" "-O0;-g3;-ggdb")

# add the executable
add_executable(trace_trimmer trace_trimmer.cpp)
# target_link_libraries(op_analysis ToolsLib)

# add_subdirectory(tools)

