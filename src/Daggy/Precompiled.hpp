#pragma once

#include <QCoreApplication>
#include <QCommandLineParser>

#include <QDebug>

#include <QFileInfo>
#include <QStandardPaths>
#include <QDateTime>

#include <QTimer>
#include <QDir>

#include <QProcessEnvironment>

#include <iostream>
#include <ostream>
#include <set>

#ifdef CONAN_BUILD
#include <kainjow/mustache.hpp>
#else
#include <mustache.hpp>
#endif

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <signal.h>
#endif
