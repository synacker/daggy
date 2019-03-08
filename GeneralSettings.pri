PRECOMPILED_HEADER = Precompiled.h

INCLUDEPATH += $$PWD


unix: {
    INCLUDEPATH += /usr/include/botan-2
}

win32: {
    LIBS += -L$$PWD/3rd-party/msvc17_64x/botan2/lib/
    INCLUDEPATH += $$PWD/3rd-party/include/

    QMAKE_TARGET_COMPANY = "Mikhail Milovidov <milovidovmikhail@gmail.com>"
    QMAKE_TARGET_PRODUCT = $$TARGET
    QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2019 Milovidov Mikhail"
}

macos: {
}


CONFIG += c++14 skip_target_version_ext

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
    BUILD_NUMBER=$$system(git rev-list tag.. --count)
    isEmpty(VERSION) {
        VERSION = 0.0.0
    }
    isEmpty($$BUILD_NUMBER) {
        BUILD_NUMBER = 0
    }
    win32: {
        VERSION="$$VERSION"."$$BUILD_NUMBER"
    }
}

DEFINES += DATAAGREGATOR_VERSION=\\\"$$VERSION\\\" PROGRAM_NAME=\\\"$$TARGET\\\" DATAAGREGATOR_BUILD_NUMBER=\\\"$$BUILD_NUMBER\\\"

