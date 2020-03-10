cmake_minimum_required(VERSION 3.16.4)
project(daggy)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_PREFIX_PATH ${CONAN_QT_ROOT})

include(conan)

add_subdirectory(src)
