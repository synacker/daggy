cmake_minimum_required(VERSION 3.16.3)

project(daggy)

set(CMAKE_AUTOMOC ON)

option(SSH2_SUPPORT "ssh2 data provider support" ON)
option(YAML_SUPPORT "yaml data sources support" ON)
option(DAGGY_CONSOLE "daggy console application" ON)
option(DAGGY_CORE_STATIC "static daggy core lib" OFF)
option(CONAN_BUILD "build with conan" OFF)
option(PACKAGE_DEPS "include daggy deps into package" OFF)

set(COMPANY_NAME "synacker")
set(APP_COPYRIGTH "Copyright \\251 2021 Mikhail Milovidov. Some Rights Reserved")

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

enable_testing()

add_subdirectory(DaggyCore)
if(DAGGY_CONSOLE)
    add_subdirectory(Daggy)
endif()

include(pack)
