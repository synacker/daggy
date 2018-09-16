PRECOMPILED_HEADER = Precompiled.h

INCLUDEPATH += $$PWD

CONFIG += c++14

equals(TEMPLATE, "lib") {
        !contains(CONFIG, staticlib) {
            win32 {
                    TARGET_EXT = .dll
            }
        }
}
DEFINES += STATICLINK

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
QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2016 Milovidov, Inc"

