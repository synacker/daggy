set(TARGET pingpong)

find_package(Qt6 COMPONENTS Core REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

enable_testing()

set(SOURCES
    main.cpp
)

qt6_add_executable(${TARGET} ${SOURCES})
include(rpath_bin)
target_link_libraries(${TARGET} PRIVATE DaggyCore)
target_precompile_headers(${TARGET} PRIVATE Precompiled.h)
