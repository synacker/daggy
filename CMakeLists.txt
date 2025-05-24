# This cmake only for developing daggy
# Use ${PROJECT_SOURCE_DIR}/cmake/conan_provider.cmake for working from IDE (see https://github.com/conan-io/cmake-conan for details)

cmake_minimum_required(VERSION 3.24.0)

message(WARNING "This CMakeLists.txt only for developing")

if (NOT CMAKE_PROJECT_TOP_LEVEL_INCLUDES)
    message(FATAL "Set CMAKE_PROJECT_TOP_LEVEL_INCLUDES with conan_provider")
endif()

project(daggy_dev)

set(PORTABLE_BUILD ON)

include(CTest)
add_subdirectory(src)
