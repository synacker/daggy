project(testcpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(testcpp ../../src/DaggyCore/tests/interface/testcpp/test.cpp)
target_link_libraries(testcpp daggy::DaggyCore)
