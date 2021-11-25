cmake_minimum_required(VERSION 3.20)
project(daggy)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_PREFIX_PATH ${CONAN_QT_ROOT})

include(conan)

set(CONAN_BUILD ON)
enable_testing()

add_subdirectory(src)
