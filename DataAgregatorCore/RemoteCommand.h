#ifndef REMOTECOMMAND_H
#define REMOTECOMMAND_H

#include <QString>

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



#endif // REMOTECOMMAND_H
