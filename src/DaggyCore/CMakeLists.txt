set(TARGET DaggyCore)

find_package(Qt6 COMPONENTS Core Network REQUIRED)

set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME application)

set(INCLUDE_PUBLIC_HEADERS
    Core.hpp
    Result.hpp
    Sources.hpp
    Types.hpp
    providers/IProvider.hpp
    providers/IFabric.hpp
    aggregators/IAggregator.hpp
    ${CMAKE_BINARY_DIR}/exports/daggycore_export.h
)

set(SOURCES ${SOURCES}
    Core.cpp
    Sources.cpp
    Errors.cpp
    providers/IProvider.cpp
    providers/CLocal.cpp
    providers/IFabric.cpp
    providers/CLocalFabric.cpp
    aggregators/IAggregator.cpp
    aggregators/CFile.cpp
    aggregators/CConsole.cpp
)

set(LINK_LIBRARIES
    Qt6::Core Qt6::Network
)

add_subdirectory(tests/local)
add_subdirectory(tests/pingpong)

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

ADD_BINARY_META(LIB)
SET_RPATH(LIB)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(${TARGET} ${SOURCES})
target_link_libraries(${TARGET} ${LINK_LIBRARIES})

include(GenerateExportHeader)
generate_export_header(${TARGET} EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/daggycore_export.h)
target_precompile_headers(${TARGET} PRIVATE Precompiled.hpp)


if(UNIX)
    if (BUILD_SHARED_LIBS)
        install(TARGETS ${TARGET} LIBRARY COMPONENT application)
        install(TARGETS ${TARGET} LIBRARY COMPONENT devel)
    else()
        install(TARGETS ${TARGET} LIBRARY COMPONENT devel)
    endif()
else()
    install(TARGETS ${TARGET} ARCHIVE COMPONENT devel)
    if (BUILD_SHARED_LIBS)
        install(TARGETS ${TARGET} RUNTIME COMPONENT application)
        install(TARGETS ${TARGET} RUNTIME COMPONENT devel)
    endif()
endif()

install(FILES ${INCLUDE_PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET} COMPONENT devel)
