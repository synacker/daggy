set(TARGET DaggyCore)

find_package(Qt5 COMPONENTS Core Network REQUIRED)

set(SOURCES
    IDataProvider.cpp
    IDataAggregator.cpp
    IDataProviderFabric.cpp
    Common.cpp
    CSsh2DataProvider.cpp
    CSsh2DataProviderFabric.cpp
    Ssh2Client.cpp
    Ssh2Types.cpp
    Ssh2Process.cpp
    Ssh2Channel.cpp
    Ssh2Types.cpp
    Ssh2Debug.cpp
    DaggyCore.cpp
    Result.cpp
)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

set(LINK_LIBRARIES
    Qt5::Core Qt5::Network
)

if(SSH2_SUPPORT)
    set(LINK_LIBRARIES ${LINK_LIBRARIES} ssh2)
endif()

if(YAML_SUPPORT)
    set(LINK_LIBRARIES ${LINK_LIBRARIES} yaml-cpp)
endif()

add_library(${TARGET} SHARED ${SOURCES})
target_link_libraries(${TARGET} ${LINK_LIBRARIES})

include(precompiled)
include(GenerateExportHeader)
generate_export_header(${TARGET} EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/daggycore_export.h)

install(TARGETS ${TARGET} DESTINATION ${LIBRARY_INSTALL_DIR})
