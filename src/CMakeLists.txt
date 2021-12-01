cmake_minimum_required(VERSION 3.22.0)
project(daggy
        DESCRIPTION "Data Aggregation Utility"
        HOMEPAGE_URL https://daggy.dev
        LANGUAGES CXX
)

set(PROJECT_VENDOR synacker)
set(PROJECT_COPYRIGTH "Copyright \\251 2021 Mikhail Milovidov. Some Rights Reserved")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(version)
include(metabin)
include(rpath)
SET_GIT_VERSION(${VERSION})


option(SSH2_SUPPORT "ssh2 data provider support" ON)
option(YAML_SUPPORT "yaml data sources support" ON)
option(DAGGY_CONSOLE "daggy console application" ON)
option(BUILD_SHARED_LIBS ON)


if(SSH2_SUPPORT)
    add_definitions(-DSSH2_SUPPORT)
endif()

if(YAML_SUPPORT)
    add_definitions(-DYAML_SUPPORT)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR}/exports)

include(compiler)

enable_testing()

add_subdirectory(DaggyCore)
if(DAGGY_CONSOLE)
    add_subdirectory(Daggy)
endif()

include(pack)



