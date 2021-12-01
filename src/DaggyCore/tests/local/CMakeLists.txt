set(TARGET DaggyCoreLocalTests)

find_package(Qt6 COMPONENTS Test REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

enable_testing()

set(SOURCES
    DaggyCoreLocalTests.cpp
    main.cpp
)


SET_RPATH()

qt6_add_executable(${TARGET} ${SOURCES})
add_test(NAME ${TARGET} COMMAND ${TARGET} WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
target_link_libraries(${TARGET} PRIVATE DaggyCore Qt6::Test)
target_precompile_headers(${TARGET} PRIVATE Precompiled.h)
