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
    VariantGettor.cpp
    DaggyCore.cpp
    LastError.cpp
)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

add_library(${TARGET} SHARED ${SOURCES})
target_link_libraries(${TARGET} Qt5::Core Qt5::Network ssh2)
include(precompiled)
include(GenerateExportHeader)
generate_export_header(${TARGET} EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/daggycore_export.h)

install(TARGETS ${TARGET} DESTINATION ${LIBRARY_INSTALL_DIR})
