project(DaggyPackageTest)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(daggy REQUIRED)

add_subdirectory(testcpp)
add_subdirectory(testc)
