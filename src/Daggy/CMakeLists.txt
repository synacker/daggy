set(TARGET daggy)

find_package(Qt5 COMPONENTS Core Network REQUIRED)

set(SOURCES
    main.cpp
    CFileDataAggregator.cpp
    CConsoleDaggy.cpp
    Common.cpp
)

if (WIN32)
    set(SOURCES ${SOURCES}
        ISystemSignalHandlerWin32.cpp
    )
else()
    set(SOURCES ${SOURCES}
        ISystemSignalHandlerUnix.cpp
    )
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/simple_multiple_servers.yaml ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/simple_multiple_servers.json ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)

add_executable(${TARGET} ${SOURCES})
target_link_libraries(${TARGET} DaggyCore Qt5::Core Qt5::Network yaml-cpp)
include(precompiled)
