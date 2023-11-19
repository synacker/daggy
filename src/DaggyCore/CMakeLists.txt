set(TARGET DaggyCore)

find_package(Qt6 COMPONENTS Core Network REQUIRED)
add_library(${TARGET})

include(rpath_lib)

set_target_properties(${TARGET} PROPERTIES AUTOMOC ON)

target_sources(${TARGET}
    PRIVATE
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
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
        ${CMAKE_CURRENT_LIST_DIR}/..
    FILES
        Core.hpp
        Core.h
        Result.hpp
        Sources.hpp
        Errors.h
        Errors.hpp
        Types.h
        Types.hpp
        providers/IProvider.hpp
        providers/IFabric.hpp
        aggregators/IAggregator.hpp
        aggregators/CFile.hpp
        aggregators/CConsole.hpp
)

target_link_libraries(${TARGET} PUBLIC Qt6::Core Qt6::Network)


if (SSH2_SUPPORT)
    if(CONAN_BUILD)
        find_package(Libssh2 REQUIRED)
        target_link_libraries(${TARGET} PUBLIC Libssh2::libssh2)
    else()
        target_link_libraries(${TARGET} PUBLIC ssh2 crypto)
    endif()

    target_sources(${TARGET}
        PRIVATE
            providers/ssh2/Ssh2Client.cpp
            providers/ssh2/Ssh2Types.cpp
            providers/ssh2/Ssh2Process.cpp
            providers/ssh2/Ssh2Channel.cpp
            providers/ssh2/Ssh2Types.cpp
            providers/ssh2/Ssh2Debug.cpp
            providers/CSsh2.cpp
            providers/CSsh2Fabric.cpp
            providers/ssh2/Ssh2Client.hpp
            providers/ssh2/Ssh2Types.hpp
            providers/ssh2/Ssh2Process.hpp
            providers/ssh2/Ssh2Channel.hpp
            providers/ssh2/Ssh2Types.hpp
            providers/ssh2/Ssh2Debug.hpp
            providers/CSsh2.hpp
            providers/CSsh2Fabric.hpp
    )
endif()

if(YAML_SUPPORT)
    find_package(yaml-cpp REQUIRED)
    target_link_libraries(${TARGET} PUBLIC yaml-cpp)
endif()


string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
configure_file(version.h.in ${CMAKE_BINARY_EXPORTS_DIR}/${TARGET}/version.h @ONLY)

include(GenerateExportHeader)
generate_export_header(${TARGET} EXPORT_FILE_NAME ${CMAKE_BINARY_EXPORTS_DIR}/${TARGET}/daggycore_export.h)
target_precompile_headers(${TARGET} PRIVATE Precompiled.hpp)

target_sources(${TARGET}
    PUBLIC FILE_SET HEADERS
    BASE_DIRS ${CMAKE_BINARY_EXPORTS_DIR}
    FILES
        ${CMAKE_BINARY_EXPORTS_DIR}/${TARGET}/version.h
        ${CMAKE_BINARY_EXPORTS_DIR}/${TARGET}/daggycore_export.h
)

if (BUILD_TESTING)
    add_subdirectory(tests/local)
    add_subdirectory(tests/pingpong)
    add_subdirectory(tests/interface)
endif()

if(UNIX)
    install(TARGETS ${TARGET} LIBRARY)
else()
    install(TARGETS ${TARGET} ARCHIVE)
    if (BUILD_SHARED_LIBS)
        install(TARGETS ${TARGET} RUNTIME)
    endif()
endif()

install(TARGETS ${TARGET} FILE_SET HEADERS)
