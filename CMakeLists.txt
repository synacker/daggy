cmake_minimum_required(VERSION 3.18.4)
project(daggy)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_PREFIX_PATH ${CONAN_QT_ROOT})

include(conan)

option(CONAN_BUILD "build with conan" ON)
option(PACKAGE_DEPS "include daggy deps into package" ON)

set(PACKAGE_DEPS ${CONAN_BUILD})

enable_testing()

add_subdirectory(src)
