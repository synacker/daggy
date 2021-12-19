set(TARGET daggy)

find_package(Qt6 COMPONENTS Core REQUIRED)

set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME application)



set(SOURCES ${SOURCES}
    main.cpp
    CConsoleDaggy.cpp
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

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/simple_sources.yaml ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/simple_sources.json ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)

add_definitions(-DAPP_ICON="${CMAKE_CURRENT_LIST_DIR}/../icons/daggy.ico")

ADD_BINARY_META()
SET_RPATH()

add_executable(${TARGET} ${SOURCES})
target_link_libraries(${TARGET} PRIVATE DaggyCore)
target_precompile_headers(${TARGET} PRIVATE Precompiled.hpp)

install(TARGETS ${TARGET} RUNTIME)
