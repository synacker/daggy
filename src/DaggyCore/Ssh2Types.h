/*
MIT License

Copyright (c) 2020 Mikhail Milovidov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once

#include <QString>
#include <QMetaType>

#include <system_error>

struct _LIBSSH2_SESSION;
typedef struct _LIBSSH2_SESSION LIBSSH2_SESSION;

struct _LIBSSH2_CHANNEL;
typedef struct _LIBSSH2_CHANNEL LIBSSH2_CHANNEL;

struct _LIBSSH2_KNOWNHOSTS;
typedef struct _LIBSSH2_KNOWNHOSTS LIBSSH2_KNOWNHOSTS;

namespace daggyssh2 {

extern const std::error_code ssh2_success;

QString defaultUser();
QString defaultKey();
QString defaultKnownHosts();

struct Ssh2Settings {
    QString user = defaultUser();
    quint16 port = 22;
    QString passphrase;
    QString key = defaultKey();
    QString keyphrase;
    QString known_hosts = defaultKnownHosts();
    unsigned int timeout = 1000;

    bool isPasswordAuth() const;
    bool isKeyAuth() const;
};

enum Ssh2Error {
    ErrorReadKnownHosts = 1,
    SessionStartupError,
    UnexpectedError,
    HostKeyInvalidError,
    HostKeyMismatchError,
    HostKeyUnknownError,
    AuthenticationError,
    FailedToOpenChannel,
    FailedToCloseChannel,
    ProcessFailedToStart,
    ChannelReadError,
    ChannelWriteError,
    TryAgain,
    ConnectionTimeoutError,
    TcpConnectionError,
    TcpConnectionRefused
};

std::error_code make_error_code(Ssh2Error ssh2_error);
}

Q_DECLARE_METATYPE(daggyssh2::Ssh2Settings)

namespace std
{
template <>
struct is_error_code_enum<daggyssh2::Ssh2Error> : true_type {};
}

