/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef REMOTECOMMAND_H
#define REMOTECOMMAND_H

#include <QString>

namespace dataagregatorcore {

struct RemoteCommand {
    enum class Status {
        NotStarted,
        Started,
        FailedToStart,
        CrashExit,
        NormalExit
    };

    struct Stream {
        enum class Type {
            Standard,
            Error
        };

        const QString command_name;
        const QString output_extension;
        const QByteArray data;
        const Type type;
    };

    const QString command_name;
    const QString command;
    const QString output_extension;
    const bool restart;
};

}

#endif // REMOTECOMMAND_H
