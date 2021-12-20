cmake_minimum_required(VERSION 3.22.0)
project(daggy
        DESCRIPTION "Data Aggregation Utility"
        HOMEPAGE_URL https://daggy.dev
        LANGUAGES CXX
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_PREFIX_PATH ${CONAN_QT_ROOT})

include(conan)

option(CONAN_BUILD "build with conan" ON)
option(PACKAGE_DEPS "include daggy deps into package" ON)

enable_testing()

add_subdirectory(src)
