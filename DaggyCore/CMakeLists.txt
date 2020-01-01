set(TARGET DaggyCore)

find_package(Qt5 COMPONENTS Core Network REQUIRED)

set(SOURCES
    IDataProvider.cpp
    Common.cpp
    CSsh2DataProvider.cpp
    Ssh2Client.cpp
    Ssh2Types.cpp
    Ssh2Process.cpp
    Ssh2Channel.cpp
    Ssh2Types.cpp
    Ssh2Debug.cpp
)

add_library(${TARGET} SHARED ${SOURCES})
target_link_libraries(${TARGET} Qt5::Core Qt5::Network ssh2)
include(precompiled)
