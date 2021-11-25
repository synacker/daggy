set(TARGET DaggyCoreLocalTests)

find_package(Qt6 COMPONENTS Test REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

enable_testing()

set(SOURCES
    DaggyCoreLocalTests.cpp
    main.cpp
)

include(rpath_bin)

qt6_add_executable(${TARGET} ${SOURCES})
add_test(NAME ${TARGET} COMMAND ${TARGET})

target_link_libraries(${TARGET} PRIVATE DaggyCore Qt6::Test)

target_precompile_headers(${TARGET} PRIVATE Precompiled.h)
