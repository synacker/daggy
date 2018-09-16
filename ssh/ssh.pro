TARGET = qssh
QT += gui network
DEFINES += QTCSSH_LIBRARY

TEMPLATE = lib

CONFIG += staticlib

include(../GeneralSettings.pri)

SOURCES = $$PWD/*.cpp

HEADERS = $$PWD/*.h \

HEADERS -= Precompiled.h

unix: {
    CONFIG += link_pkgconfig
    PKGCONFIG += botan-2
    INCLUDEPATH += usr/include/botan-2
}

win32: {
    INCLUDEPATH += $$PWD/../3rd-party/win/botan2/include/botan-2
    LIBS += -ladvapi32 -luser32 -lws2_32
}
