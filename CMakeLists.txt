cmake_minimum_required(VERSION 3.21.0)
project(daggy
        DESCRIPTION "Data Aggregation Utility"
        HOMEPAGE_URL https://github.com/synacker/daggy
        LANGUAGES CXX
)

cmake_minimum_required(VERSION 3.21.0)

option(CONAN_BUILD "build with conan" ON)
option(PACKAGE_DEPS "include daggy deps into package" ON)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE ${CMAKE_CURRENT_SOURCE_DIR}
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings})

include(CTest)
add_subdirectory(src)