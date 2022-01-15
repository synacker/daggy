project(testcpp LANGUAGES CXX)

add_executable(testcpp test.cpp)
target_link_libraries(testcpp DaggyCore)
