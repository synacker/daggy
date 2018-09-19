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
    CDataSources.cpp \
    CSourceServer.cpp \
    CSourceServerCommand.cpp \
    CDataAgregator.cpp \
    CSourceServerSshConnection.cpp \
    IDataSourcesReciever.cpp \
    CSourceServerAuthorization.cpp

HEADERS +=\
        dataagregatorcore_global.h \
    Precompiled.h \
    CDataAgregator.h \
    CDataSources.h \
    CSourceServer.h \
    CSourceServerCommand.h \
    CSourceServerSshConnection.h \
    IDataSourcesReciever.h \
    IDataSourcesSender.h \
    CSourceServerAuthorization.h

DEPENDPATH += $$PWD/../ssh
LIBS += -lqssh

unix: {
    CONFIG += link_pkgconfig
    PKGCONFIG += botan-2
}

win32: {
    LIBS += -ladvapi32 -luser32 -lws2_32 -lbotan
}

