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

#include <optional>
#include <chrono>
#include <QString>
#include <QByteArray>
#include <QVariantMap>

#include "Types.h"

namespace daggy {
namespace sources {
namespace commands {
namespace streams {
struct Meta {
    QString session;
    std::chrono::time_point<std::chrono::system_clock> start_time;
    QString extension;
    DaggyStreamTypes type;

    std::uint64_t seq_num;
    std::chrono::time_point<std::chrono::system_clock> time;
};
}

struct Stream {
    streams::Meta meta;
    QByteArray part;
};

struct DAGGYCORE_EXPORT Properties {
    QString extension;
    QString exec;
    QVariantMap parameters = {};
    bool restart = false;

    bool operator==(const Properties& other) const;
};
}

using Commands = QMap<QString, commands::Properties>;
using Command = QPair<QString, commands::Properties>;

struct DAGGYCORE_EXPORT Properties {
    QString type;
    QString host;
    Commands commands;
    bool reconnect = false;
    QVariantMap parameters = {};

    bool operator==(const Properties& other) const;
};
}

using Sources = QMap<QString, sources::Properties>;
using Source = QPair<QString, sources::Properties>;


namespace sources {
namespace convertors {

DAGGYCORE_EXPORT std::optional<Sources> json(const QString& data);
DAGGYCORE_EXPORT std::optional<Sources> yaml(const QString& data);

DAGGYCORE_EXPORT std::optional<Sources> json(const QString& data, QString& error) noexcept;
DAGGYCORE_EXPORT std::optional<Sources> yaml(const QString& data, QString& error) noexcept;

}
}
}

Q_DECLARE_METATYPE(daggy::sources::commands::Properties)
Q_DECLARE_METATYPE(daggy::sources::Properties)

Q_DECLARE_METATYPE(daggy::sources::commands::Stream)
Q_DECLARE_METATYPE(daggy::sources::Command);
Q_DECLARE_METATYPE(daggy::sources::Commands);
Q_DECLARE_METATYPE(daggy::Source);
Q_DECLARE_METATYPE(daggy::Sources);

