TARGET = qssh
QT += gui network
DEFINES += QTCSSH_LIBRARY

TEMPLATE = lib

CONFIG += staticlib

include(../GeneralSettings.pri)

SOURCES = $$PWD/*.cpp

HEADERS = $$PWD/*.h \

HEADERS -= Precompiled.h
