set(TARGET DaggyCore)

find_package(Qt5 COMPONENTS Core Network REQUIRED)

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

set(INCLUDE_HEADERS
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

set(SOURCES
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
qt5_wrap_cpp(MOC_SOURCES Command.h)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

set(LINK_LIBRARIES
    Qt5::Core Qt5::Network
)

if (SSH2_SUPPORT)
    list(APPEND SOURCES ${SSH2_SOURCES})
    list(APPEND INCLUDE_HEADERS ${SSH2_PUBLIC})
    set(LINK_LIBRARIES ${LINK_LIBRARIES} ssh2)
endif()


if(YAML_SUPPORT)
    list(APPEND SOURCES ${YAML_SOURCES})
    list(APPEND INCLUDE_HEADERS ${YAML_PUBLIC})
    set(LINK_LIBRARIES ${LINK_LIBRARIES} yaml-cpp)
endif()

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

install(TARGETS ${TARGET} DESTINATION ${LIBRARY_INSTALL_DIR})
install(FILES ${INCLUDE_PUBLIC} DESTINATION ${INCLUDE_INSTALL_DIR}/${TARGET})
