/*
MIT License

Copyright (c) 2021 Mikhail Milovidov

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

#include "daggycore_export.h"

#include <QString>
#include <QByteArray>
#include <QVariantMap>

namespace daggy {
namespace sources {
namespace command {

enum State {
    NotStarted,
    Starting,
    Started,
    FailedToStart,
    Finishing,
    Finished
};

namespace stream {
enum Type {
    Standard,
    Error
};
}

struct Stream {
    QString extension;
    QByteArray data;
    stream::Type type;
};

struct DAGGYCORE_EXPORT Properties {
    QString extension;
    QString exec;
    QVariantMap parameters = {};
    bool restart = false;

    bool operator==(const Properties& other) const;
};
}

using Commands = std::map<QString, command::Properties>;
using Command = Commands::value_type;

struct DAGGYCORE_EXPORT Properties {
    QString type;
    QString host;
    Commands commands;
    bool reconnect = false;
    QVariantMap parameters = {};

    bool operator==(const Properties& other) const;
};
}

using Sources = std::map<QString, sources::Properties>;
using Source = Sources::value_type;
}

Q_ENUMS(daggy::sources::command::stream::Type daggy::sources::command::State)

Q_DECLARE_METATYPE(daggy::sources::command::Properties)
Q_DECLARE_METATYPE(daggy::sources::Properties)

Q_DECLARE_METATYPE(daggy::sources::command::Stream)
Q_DECLARE_METATYPE(daggy::sources::Command);
Q_DECLARE_METATYPE(daggy::sources::Commands);
Q_DECLARE_METATYPE(daggy::Source);
Q_DECLARE_METATYPE(daggy::Sources);

