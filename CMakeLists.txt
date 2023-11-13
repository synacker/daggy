# This cmake only for developing daggy
# Use ${PROJECT_SOURCE_DIR}/cmake/conan_provider.cmake for working from IDE.
cmake_minimum_required(VERSION 3.24.0)

message(WARNING "This CMakeLists.txt only for developing")

if (NOT CMAKE_PROJECT_TOP_LEVEL_INCLUDES)
    message(FATAL "Set CMAKE_PROJECT_TOP_LEVEL_INCLUDES with conan_provider")
endif()

project(daggy_dev)

option(CONAN_BUILD "build with conan" ON)
add_definitions(-DCONAN_BUILD)
option(PACKAGE_DEPS "include daggy deps into package" ON)

include(CTest)
add_subdirectory(src)
