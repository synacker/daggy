set(TARGET DaggyCore)

find_package(Qt6 COMPONENTS Core Network REQUIRED)

set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME application)

if(APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path/daggy")
elseif(UNIX)
    set(CMAKE_INSTALL_RPATH "$ORIGIN/daggy;")
endif()

set(SSH2_SOURCES
    Ssh2Client.cpp
    Ssh2Types.cpp
    Ssh2Process.cpp
    Ssh2Channel.cpp
    Ssh2Types.cpp
    Ssh2Debug.cpp
    CSsh2DataProvider.cpp
    CSsh2DataProviderFabric.cpp
)

set(SSH2_PUBLIC
    CSsh2DataProviderFabric.h
)

set(YAML_PUBLIC
    CYamlDataSourcesConvertor.h
)

set(INCLUDE_PUBLIC_HEADERS
    CJsonDataSourcesConvertor.h
    CYamlDataSourcesConvertor.h
    CLocalDataProvider.h
    CSsh2DataProviderFabric.h
    Command.h
    Common.h
    DaggyCore.h
    IDataAggregator.h
    IDataProviderFabric.h
    IDataSourceConvertor.h
    Result.h
    ${CMAKE_BINARY_DIR}/exports/daggycore_export.h
)

set(YAML_SOURCES
    CYamlDataSourcesConvertor.cpp
)

set(SOURCES ${SOURCES}
    IDataProvider.cpp
    IDataAggregator.cpp
    IDataProviderFabric.cpp
    Common.cpp
    DaggyCore.cpp
    Result.cpp
    CJsonDataSourcesConvertor.cpp
    IDataSourceConvertor.cpp
    CLocalDataProvider.cpp
    CLocalDataProvidersFabric.cpp
)
qt6_wrap_cpp(MOC_SOURCES Command.h)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

set(LINK_LIBRARIES
    Qt6::Core Qt6::Network
)

add_subdirectory(tests/local)

if (SSH2_SUPPORT)
    #add_subdirectory(tests/ssh2)
    if (CONAN_BUILD)
        set(LINK_LIBRARIES ${LINK_LIBRARIES} CONAN_PKG::libssh2)
    else()
        set(LINK_LIBRARIES ${LINK_LIBRARIES} ssh2)
    endif()
    list(APPEND SOURCES ${SSH2_SOURCES})
    list(APPEND INCLUDE_HEADERS ${SSH2_PUBLIC})
endif()

if(YAML_SUPPORT)
    if (CONAN_BUILD)
        set(LINK_LIBRARIES ${LINK_LIBRARIES} CONAN_PKG::yaml-cpp)
    else()
        set(LINK_LIBRARIES ${LINK_LIBRARIES} yaml-cpp)
    endif()
    list(APPEND SOURCES ${YAML_SOURCES})
    list(APPEND INCLUDE_HEADERS ${YAML_PUBLIC})
endif()

set(APP_NAME ${TARGET})
set(APP_DESCRIPTION "Daggy core library")
set(FILETYPE "VFT_DLL")

include(version_meta)

if (DAGGY_CORE_STATIC)
    add_definitions(-DDAGGYCORE_STATIC_DEFINE)
    add_library(${TARGET} STATIC ${SOURCES} ${MOC_SOURCES})
else()
    add_library(${TARGET} SHARED ${SOURCES} ${MOC_SOURCES})
endif()
target_link_libraries(${TARGET} ${LINK_LIBRARIES})

include(precompiled)
include(GenerateExportHeader)
generate_export_header(${TARGET} EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/daggycore_export.h)


if(UNIX)
    if (DAGGY_CORE_STATIC)
        install(TARGETS ${TARGET} LIBRARY COMPONENT devel)
    else()
        install(TARGETS ${TARGET} LIBRARY COMPONENT application)
        install(TARGETS ${TARGET} LIBRARY COMPONENT devel)
    endif()
else()
    install(TARGETS ${TARGET} ARCHIVE COMPONENT devel)
    if (NOT DAGGY_CORE_STATIC)
        install(TARGETS ${TARGET} RUNTIME COMPONENT application)
        install(TARGETS ${TARGET} RUNTIME COMPONENT devel)
    endif()
endif()

install(FILES ${INCLUDE_PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET} COMPONENT devel)
