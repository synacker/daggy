PRECOMPILED_HEADER = Precompiled.h

INCLUDEPATH += $$PWD


unix: !macx {
    CONFIG += link_pkgconfig
    PKGCONFIG += botan-2 yaml-cpp
}

macx: {
    INCLUDEPATH += /usr/local/include/botan-2
    INCLUDEPATH += /usr/local/include/

    LIBS += -L/usr/local/lib -lbotan-2 -lyaml-cpp
}

win32: {
    LIBS += -L$$PWD/3rd-party/msvc17_64x/botan2/lib/ -L$$PWD/3rd-party/msvc17_64x/yaml-cpp/
    INCLUDEPATH += $$PWD/3rd-party/include/

    QMAKE_TARGET_COMPANY = "Mikhail Milovidov <milovidovmikhail@gmail.com>"
    QMAKE_TARGET_PRODUCT = $$TARGET
    QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2019 Milovidov Mikhail"
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
  BUILD_NUMBER = $$system(git rev-list $${VERSION}.. --count)
}

isEmpty(BUILD_NUMBER) {
  BUILD_NUMBER = $$system(git rev-list $${VERSION}.. --count)
}

win32: {
    VERSION="$$VERSION"."$$BUILD_NUMBER"
}


DEFINES += DAGGY_VERSION=\\\"$$VERSION\\\" \
           PROGRAM_NAME=\\\"$$TARGET\\\" \
           DAGGY_BUILD_NUMBER=\\\"$$BUILD_NUMBER\\\"

