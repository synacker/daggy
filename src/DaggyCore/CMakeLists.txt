set(TARGET DaggyCore)

find_package(Qt6 COMPONENTS Core Network REQUIRED)

set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME application)

include(rpath_lib)

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
    DataSource.cpp
    Command.cpp
)
qt6_wrap_cpp(MOC_SOURCES Command.h)



set(LINK_LIBRARIES
    Qt6::Core Qt6::Network
)

add_subdirectory(tests/local)

if (SSH2_SUPPORT)
    set(LINK_LIBRARIES ${LINK_LIBRARIES} ssh2 crypto)
    list(APPEND SOURCES ${SSH2_SOURCES})
    list(APPEND INCLUDE_HEADERS ${SSH2_PUBLIC})
endif()

if(YAML_SUPPORT)
    find_package(yaml-cpp REQUIRED)
    set(LINK_LIBRARIES ${LINK_LIBRARIES} yaml-cpp)
    list(APPEND SOURCES ${YAML_SOURCES})
    list(APPEND INCLUDE_HEADERS ${YAML_PUBLIC})
endif()



set(APP_NAME ${TARGET})
set(APP_DESCRIPTION "Daggy core library")
set(FILETYPE "VFT_DLL")

include(version_meta)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

add_library(${TARGET} ${SOURCES} ${MOC_SOURCES})
target_link_libraries(${TARGET} ${LINK_LIBRARIES})

include(GenerateExportHeader)
generate_export_header(${TARGET} EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/daggycore_export.h)

target_precompile_headers(${TARGET} PRIVATE Precompiled.h)

if(UNIX)
    if (BUILD_SHARED_LIBS)
        install(TARGETS ${TARGET} LIBRARY COMPONENT application)
        install(TARGETS ${TARGET} LIBRARY COMPONENT devel)
    else()
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
