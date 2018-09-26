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

        const QString commandName;
        const QString outputExtension;
        const QByteArray data;
        const Type type;
    };

    const QString commandName;
    const QString command;
    const QString outputExtension;
    const bool restart;
};



#endif // REMOTECOMMAND_H
