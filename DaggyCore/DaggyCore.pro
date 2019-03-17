#-------------------------------------------------
#
# Project created by QtCreator 2016-08-04T19:35:23
#
#-------------------------------------------------

TARGET = DaggyCore
TEMPLATE = lib

QT += core network

include(../GeneralSettings.pri)

DEFINES += DAGGYCORE_LIBRARY

SOURCES += \
    CDaggy.cpp \
    IRemoteServer.cpp \
    CSshRemoteServer.cpp \
    IRemoteAgregator.cpp \
    CDefaultRemoteServersFabric.cpp \
    CLocalRemoteServer.cpp \
    CDataSourcesFabric.cpp

HEADERS +=\
    Precompiled.h \
    CDaggy.h \
    IRemoteServer.h \
    CSshRemoteServer.h \
    IRemoteServersFabric.h \
    DataSource.h \
    RemoteCommand.h \
    IRemoteAgregator.h \
    CDefaultRemoteServersFabric.h \
    IRemoteAgregatorReciever.h \
    RemoteConnectionStatus.h \
    CLocalRemoteServer.h \
    CDataSourcesFabric.h \
    daggycore_global.h

DEPENDPATH += $$PWD/../ssh
LIBS += -lqssh

unix: {
    target.path = $$LIBDIR
    INSTALLS += target
}


win32: {
    LIBS += -ladvapi32 -luser32 -lws2_32 -lbotan -lyaml-cpp
    QMAKE_TARGET_DESCRIPTION = "DaggyCore - lib for runing multiple ssh commands on multiple servers simultaneously."
}

