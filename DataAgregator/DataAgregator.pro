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

    RC_ICONS = dataagregator.ico
    QMAKE_TARGET_DESCRIPTION = "DataAgregator - utility that can run and save output from multiple ssh or local commands on multiple servers simultaneously."
}
unix: {

    LIBS += -lbotan-2
    SOURCES += ISystemSignalsHandlerUnix.cpp

    target.path = $$PREFIX/bin/
    INSTALLS += target
}


DEPENDPATH += $$PWD/../DataAgregatorCore
DEPENDPATH += $$PWD/../ssh



