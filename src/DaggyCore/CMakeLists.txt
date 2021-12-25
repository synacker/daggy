set(TARGET DaggyCore)

find_package(Qt6 COMPONENTS Core Network REQUIRED)

set(INCLUDE_BASE_HEADERS
    Core.hpp
    Core.h
    Result.hpp
    Sources.hpp
    Errors.h
    Errors.hpp
    Types.h
    Types.hpp
    daggycore_export.h
)

set(INCLUDE_PROVIDERS_HEADERS
    providers/IProvider.hpp
    providers/IFabric.hpp
)

set(INCLUDE_AGGREGATORS_HEADERS
    aggregators/IAggregator.hpp
    aggregators/CFile.hpp
    aggregators/CConsole.hpp
)

set(HEADERS
    ${INCLUDE_BASE_HEADERS}
    ${INCLUDE_PROVIDERS_HEADERS}
    ${INCLUDE_AGGREGATORS_HEADERS}
)

set(SOURCES ${SOURCES}
    Core.cpp
    core_ansi_c.cpp
    Sources.cpp
    Errors.cpp
    providers/IProvider.cpp
    providers/CLocal.cpp
    providers/IFabric.cpp
    providers/CLocalFabric.cpp
    aggregators/IAggregator.cpp
    aggregators/CFile.cpp
    aggregators/CConsole.cpp
    aggregators/CCallback.cpp
)

set(LINK_LIBRARIES
    Qt6::Core Qt6::Network
)

add_subdirectory(tests/local)
add_subdirectory(tests/pingpong)
add_subdirectory(tests/interface)

if (SSH2_SUPPORT)
    if(CONAN_BUILD)
        find_package(Libssh2 REQUIRED)
        set(LINK_LIBRARIES ${LINK_LIBRARIES} Libssh2::libssh2)
    else()
        set(LINK_LIBRARIES ${LINK_LIBRARIES} ssh2 crypto)
    endif()

    set(SOURCES ${SOURCES}
        providers/ssh2/Ssh2Client.cpp
        providers/ssh2/Ssh2Types.cpp
        providers/ssh2/Ssh2Process.cpp
        providers/ssh2/Ssh2Channel.cpp
        providers/ssh2/Ssh2Types.cpp
        providers/ssh2/Ssh2Debug.cpp
        providers/CSsh2.cpp
        providers/CSsh2Fabric.cpp
    )
endif()

if(YAML_SUPPORT)
    find_package(yaml-cpp REQUIRED)
    set(LINK_LIBRARIES ${LINK_LIBRARIES} yaml-cpp)
endif()

if(WIN32)
    add_definitions(-DNOMINMAX)
endif()

ADD_BINARY_META(LIB)
SET_RPATH(LIB)

add_library(${TARGET} ${SOURCES} ${HEADERS})
target_link_libraries(${TARGET} ${LINK_LIBRARIES})

include(GenerateExportHeader)
generate_export_header(${TARGET} EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/daggycore_export.h)
target_precompile_headers(${TARGET} PRIVATE Precompiled.hpp)


if(UNIX)
    install(TARGETS ${TARGET} LIBRARY)
else()
    install(TARGETS ${TARGET} ARCHIVE)
    if (BUILD_SHARED_LIBS)
        install(TARGETS ${TARGET} RUNTIME)
        install(TARGETS ${TARGET} RUNTIME)
    endif()
endif()

install(FILES ${INCLUDE_BASE_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET})
install(FILES ${INCLUDE_AGGREGATORS_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET}/aggregators)
install(FILES ${INCLUDE_PROVIDERS_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET}/providers)
