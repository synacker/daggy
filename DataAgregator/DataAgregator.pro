TARGET = DataAgregator
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

win32: {
    SOURCES += ISystemSignalsHandlerWin32.cpp
    LIBS += -ladvapi32 -luser32
}
unix: {
    SOURCES += ISystemSignalsHandlerUnix.cpp

    target.path = $$PREFIX/bin/
    INSTALLS += target
}

LIBS += -lDataAgregatorCore -lqssh -lbotan-2

DEPENDPATH += $$PWD/../DataAgregatorCore
DEPENDPATH += $$PWD/../ssh



QMAKE_TARGET_DESCRIPTION = "Console application for data agregation"


