#ifndef PRECOMPILED_H
#define PRECOMPILED_H

#include <QDir>
#include <QFile>

#include <QtEndian>

#include <QCoreApplication>

#include <QString>
#include <QRegExp>

#include <QTimer>

#include <QList>

#include <QMutex>
#include <QMutexLocker>

#include <QNetworkProxy>
#include <QTcpSocket>

#include <QThread>

#include <limits>

#include <cctype>


#ifdef USE_SYSTEM_BOTAN
#include <botan/hmac.h>
#include <botan/ctr.h>
#include <botan/cbc.h>
#include <botan/ecdsa.h>
#include <botan/dsa.h>
#include <botan/rsa.h>
#include <botan/pubkey.h>
#include <botan/ecdh.h>
#include <botan/dh.h>
#endif

#include <botan/botan.h>




#endif // PRECOMPILED_H
