set(TARGET pingpong)

find_package(Qt6 COMPONENTS Core REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

enable_testing()

set(SOURCES
    main.cpp
)

SET_RPATH()

qt6_add_executable(${TARGET} ${SOURCES})
target_link_libraries(${TARGET} PRIVATE DaggyCore Qt6::Test)
target_precompile_headers(${TARGET} PRIVATE Precompiled.h)
