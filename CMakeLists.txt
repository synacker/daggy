cmake_minimum_required(VERSION 3.16.0)

project(daggy)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/exports)

include(compiler)
include(version)

add_subdirectory(DaggyCore)
add_subdirectory(Daggy)

