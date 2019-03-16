TARGET = daggy
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

QT += core network


include(../GeneralSettings.pri)

SOURCES += main.cpp \
    CApplicationSettings.cpp \
    CConsoleDaggy.cpp \
    CFileDataSourcesReciever.cpp


HEADERS += \
    Precompiled.h \
    CApplicationSettings.h \
    ISystemSignalsHandler.h \
    CConsoleDaggy.h \
    CFileDataSourcesReciever.h


LIBS += -lDaggyCore

DAGGY_DESCRIPTION = "Daggy - Data Aggregation Utility. Application that can run multiple commands on remote servers simultaneously and save output locally."

win32: {
    SOURCES += ISystemSignalsHandlerWin32.cpp
    LIBS += -ladvapi32 -luser32

    RC_ICONS = daggy.ico
    QMAKE_TARGET_DESCRIPTION = $$DAGGY_DESCRIPTION
}

unix: {
    SOURCES += ISystemSignalsHandlerUnix.cpp

    target.path = $$PREFIX/bin/
    INSTALLS += target
}


DEPENDPATH += $$PWD/../DaggyCore
DEPENDPATH += $$PWD/../ssh

DEFINES += APP_DESCRIPTION=\"\\\"$${DAGGY_DESCRIPTION}\\\"\"



