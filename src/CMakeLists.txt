cmake_minimum_required(VERSION 3.24)

set(CMAKE_PROJECT_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_PROJECT_MODULES})

include(version)
SET_VERSION()

project(daggy
        DESCRIPTION "Data Aggregation Utility"
        HOMEPAGE_URL https://github.com/synacker/daggy
        LANGUAGES CXX
        VERSION ${VERSION}
)

set(CMAKE_BINARY_EXPORTS_DIR ${CMAKE_BINARY_DIR}/exports/)
include_directories(${CMAKE_BINARY_EXPORTS_DIR})

set(PROJECT_VENDOR synacker)
set(PROJECT_COPYRIGTH "Copyright \\251 2025 synacker. Some Rights Reserved")

option(PORTABLE_BUILD "portable install packages" OFF)
option(APPONLY_BUILD "build only application with static deps linkage" OFF)
option(CONAN_BUILD "build with conan specific changes" OFF)

option(SSH2_SUPPORT "ssh2 data provider support" ON)
option(YAML_SUPPORT "yaml data sources support" ON)
option(CONSOLE "daggy console application" ON)

if (APPONLY_BUILD)
    set(BUILD_SHARED_LIBS OFF)
    set(CONAN_BUILD ON)
    set(PORTABLE_BUILD OFF)
    set(CONSOLE ON)
endif()

if (PORTABLE_BUILD)
    set(CONAN_BUILD ON)
    set(BUILD_SHARED_LIBS ON)
endif()

if(SSH2_SUPPORT)
    add_definitions(-DSSH2_SUPPORT)
endif()

if(CONAN_BUILD)
    add_definitions(-DCONAN_BUILD)
endif()

if(YAML_SUPPORT)
    add_definitions(-DYAML_SUPPORT)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include(compiler)
include(CTest)

add_subdirectory(DaggyCore)
if(CONSOLE)
    add_subdirectory(Daggy)
endif()

include(pack)
