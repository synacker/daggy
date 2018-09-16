PRECOMPILED_HEADER = Precompiled.h

INCLUDEPATH += $$PWD


unix: {
    INCLUDEPATH += usr/include/botan-2
}

win32: {
    LIBS += -L$$PWD/3rd-party/win/botan2/lib/
    INCLUDEPATH += $$PWD/3rd-party/win/botan2/include/botan-2
}


CONFIG += c++14


isEmpty(DESTDIR) {
        CONFIG(debug, debug|release) {
            DESTDIR=$$OUT_PWD/../Debug
        }

        CONFIG(release, debug|release) {
            DESTDIR=$$OUT_PWD/../Release
        }
}

LIBS += -L$$DESTDIR

isEmpty(VERSION) {
    VERSION = $$system(git describe --abbrev=0 --tags)
}

DEFINES += DATAAGREGATOR_VERSION=\\\"$$VERSION\\\" PROGRAM_NAME=\\\"$$TARGET\\\"

QMAKE_TARGET_COMPANY = "Mikhail Milovidov"
QMAKE_TARGET_PRODUCT = $$TARGET
QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2016-2018 Milovidov, Inc"

