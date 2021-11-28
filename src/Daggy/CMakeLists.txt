set(TARGET daggy)

find_package(Qt6 COMPONENTS Core REQUIRED)

set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME application)

include(rpath_bin)

set(SOURCES ${SOURCES}
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

add_definitions(-DAPP_ICON="${CMAKE_CURRENT_LIST_DIR}/../icons/daggy.ico")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/simple_multiple_servers.yaml ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/simple_multiple_servers.json ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)

set(APP_NAME "${TARGET}")
set(APP_DESCRIPTION "app")
set(FILETYPE "VFT_APP")

include(version_meta)

qt6_add_executable(${TARGET} WIN32 MACOSX_BUNDLE ${SOURCES})
target_link_libraries(${TARGET} PRIVATE DaggyCore)
target_precompile_headers(${TARGET} PRIVATE Precompiled.h)

install(TARGETS ${TARGET} RUNTIME)
install(TARGETS ${TARGET} BUNDLE)
