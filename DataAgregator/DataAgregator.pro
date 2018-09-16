TARGET = dataagregator
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

QT += core network


include(../GeneralSettings.pri)

SOURCES += main.cpp \
    CApplicationSettings.cpp \
    CConsoleDataAgregator.cpp \
    CFileDataSourcesReciever.cpp


HEADERS += \
    Precompiled.h \
    CApplicationSettings.h \
    ISystemSignalsHandler.h \
    CConsoleDataAgregator.h \
    CFileDataSourcesReciever.h


LIBS += -lDataAgregatorCore

win32: {
    SOURCES += ISystemSignalsHandlerWin32.cpp
    LIBS += -ladvapi32 -luser32 -lbotan
}
unix: {

    LIBS += -lbotan-2
    SOURCES += ISystemSignalsHandlerUnix.cpp

    target.path = $$PREFIX/bin/
    INSTALLS += target
}


DEPENDPATH += $$PWD/../DataAgregatorCore
DEPENDPATH += $$PWD/../ssh


QMAKE_TARGET_DESCRIPTION = "Console application for data agregation"


