cmake_minimum_required(VERSION 4.0.0)

message(WARNING "This CMakeLists.txt only for developing")
project(daggy_dev)

set(PORTABLE_BUILD ON)

include(CTest)
add_subdirectory(src)
