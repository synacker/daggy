set(TARGET daggy)

find_package(Qt5 COMPONENTS Core Network REQUIRED)

set(SOURCES
    main.cpp
    CFileDataAggregator.cpp
)

add_executable(${TARGET} ${SOURCES})
target_link_libraries(${TARGET} DaggyCore Qt5::Core Qt5::Network ssh2)
include(precompiled)
