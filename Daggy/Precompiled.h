#pragma once

#include <QCoreApplication>
#include <QCommandLineParser>

#include <QDebug>

#include <QFileInfo>
#include <QStandardPaths>
#include <QDateTime>

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <signal.h>
#endif
