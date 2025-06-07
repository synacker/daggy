cmake_minimum_required(VERSION 3.31)
project(DaggyPackageTest)

find_package(Qt6 COMPONENTS Core Network REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(DaggyCore REQUIRED)

add_subdirectory(testcpp)
add_subdirectory(testc)
