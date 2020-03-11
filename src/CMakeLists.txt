cmake_minimum_required(VERSION 3.16.4)

project(daggy)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(SSH2_SUPPORT "ssh2 data provider support" ON)
option(YAML_SUPPORT "yaml data sources support" ON)
option(DAGGY_CONSOLE "daggy console application" ON)
option(DAGGY_CORE_STATIC "static daggy core lib" OFF)

if(SSH2_SUPPORT)
    add_definitions(-DSSH2_SUPPORT)
endif()

if(YAML_SUPPORT)
    add_definitions(-DYAML_SUPPORT)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR}/exports)

include(compiler)
include(version)

add_subdirectory(DaggyCore)
if(DAGGY_CONSOLE)
    add_subdirectory(Daggy)
endif()
