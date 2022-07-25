cmake_minimum_required(VERSION 3.21.0)
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
SET_GIT_VERSION()


option(SSH2_SUPPORT "ssh2 data provider support" ON)
option(YAML_SUPPORT "yaml data sources support" ON)
option(CONSOLE "daggy console application" ON)
option(BUILD_SHARED_LIBS ON)


if(SSH2_SUPPORT)
    add_definitions(-DSSH2_SUPPORT)
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



