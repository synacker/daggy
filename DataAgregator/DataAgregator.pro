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
    !UseSystemBotan {
        LIBS += -lrt $$QMAKE_LIBS_DYNLOAD
    }

    target.path = $$PREFIX/bin/
    INSTALLS += target
}

LIBS += -lDataAgregatorCore -lQSshLib

DEPENDPATH += $$PWD/../DataAgregatorCore
DEPENDPATH += $$PWD/../QSshLib



QMAKE_TARGET_DESCRIPTION = "Console application for data agregation"


