TARGET = QSshLib
TEMPLATE = lib

QT += core network
CONFIG += staticlib

include(../GeneralSettings.pri)
DEFINES += QSSH_LIBRARY

SOURCES = $$PWD/*.cpp

HEADERS = $$PWD/*.h \
    Precompiled.h


!UseSystemBotan: {
    include($$PWD/3rdparty/botan/botan.pri)
}
