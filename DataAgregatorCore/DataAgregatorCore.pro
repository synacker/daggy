#-------------------------------------------------
#
# Project created by QtCreator 2016-08-04T19:35:23
#
#-------------------------------------------------

TARGET = DataAgregatorCore
TEMPLATE = lib

QT += core network

include(../GeneralSettings.pri)

DEFINES += DATAAGREGATORCORE_LIBRARY

SOURCES += \
    CDataAgregator.cpp \
    IRemoteServer.cpp \
    CSshRemoteServer.cpp \
    DataSource.cpp \
    IRemoteAgregator.cpp \
    CDefaultRemoteServersFabric.cpp \
    CLocalRemoteServer.cpp

HEADERS +=\
        dataagregatorcore_global.h \
    Precompiled.h \
    CDataAgregator.h \
    IRemoteServer.h \
    CSshRemoteServer.h \
    IRemoteServersFabric.h \
    DataSource.h \
    RemoteCommand.h \
    IRemoteAgregator.h \
    CDefaultRemoteServersFabric.h \
    IRemoteAgregatorReciever.h \
    RemoteConnectionStatus.h \
    CLocalRemoteServer.h

DEPENDPATH += $$PWD/../ssh
LIBS += -lqssh

unix: {
    CONFIG += link_pkgconfig
    PKGCONFIG += botan-2
}

win32: {
    LIBS += -ladvapi32 -luser32 -lws2_32 -lbotan
}

QMAKE_TARGET_DESCRIPTION = "DataAgregatorCore - lib for runing multiple ssh commands on multiple servers simultaneously."
