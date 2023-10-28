# This cmake only for developing daggy
cmake_minimum_required(VERSION 3.24.0)
set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/conan_provider.cmake")
set(QT_CREATOR_SKIP_CONAN_SETUP ON)

project(daggy
        DESCRIPTION "Data Aggregation Utility"
        HOMEPAGE_URL https://github.com/synacker/daggy
        LANGUAGES CXX
)

option(CONAN_BUILD "build with conan" ON)
option(PACKAGE_DEPS "include daggy deps into package" OFF)

include(CTest)
add_subdirectory(src)