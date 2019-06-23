/*
Copyright 2017-2018 Mikhail Milovidov <milovidovmikhail@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Precompiled.h"
#include "CFileDataSourcesReciever.h"
#include "CApplicationSettings.h"

using namespace daggycore;

CFileDataSourcesReciever::CFileDataSourcesReciever(const QString& output_folder, QObject* parent_ptr)
    : IRemoteAgregatorReciever(parent_ptr)
    , output_folder_path_(createOutputFolder(output_folder))
{
    console_message_type_ = QMetaEnum::fromType<CFileDataSourcesReciever::ConsoleMessageType>();
    printAppStatus("Start receiver");
}

CFileDataSourcesReciever::~CFileDataSourcesReciever()
{
    for (const QString& serverId : output_files_.keys())
        for (const QString& commandId : output_files_[serverId].keys())
            closeOutputFile(serverId, commandId);
    printAppStatus("Stop receiver");
}

void CFileDataSourcesReciever::onConnectionStatusChanged(const QString server_name,
                                                         const RemoteConnectionStatus status,
                                                         const QString message)
{
    QString print_message;
    switch (status) {
    case RemoteConnectionStatus::NotConnected:
    case RemoteConnectionStatus::Disconnected:
        print_message =  "Server disconnected";
        break;
    case RemoteConnectionStatus::Connected:
        print_message = "Server connected";
        break;
    case RemoteConnectionStatus::ConnectionError:
        print_message = message;
        break;
    }
    printServerMessage(ConnStatus, server_name, print_message);
}

void CFileDataSourcesReciever::onRemoteCommandStatusChanged(const QString server_name,
                                                            const RemoteCommand remote_command,
                                                            const RemoteCommand::Status status,
                                                            const int exit_code)
{
    QString print_message;

    if (status == RemoteCommand::Status::Started) {
        createOutputFile(server_name, remote_command.command_name, remote_command.output_extension);
    } else {
        closeOutputFile(server_name, remote_command.command_name);
    }

    switch (status) {
    case RemoteCommand::Status::Started:
        print_message = "Command started";
        break;
    case RemoteCommand::Status::FailedToStart:
        print_message = "Cann't start command";
        break;
    case RemoteCommand::Status::NormalExit:
        print_message = QString("Command was finished with code %2").arg(exit_code);
        break;
    case RemoteCommand::Status::CrashExit:
        print_message = "Command was crashed";
        break;
    default:
        print_message = "Error state";
    }
    printCommandMessage(CommStatus, server_name, remote_command.command_name, print_message);
}

void CFileDataSourcesReciever::onNewRemoteCommandStream(const QString server_name,
                                                        const RemoteCommand::Stream stream)
{
    switch (stream.type)
    {
    case RemoteCommand::Stream::Type::Standard:
        writeToFile(server_name, stream.command_name, stream.data);
        break;
    case RemoteCommand::Stream::Type::Error:
        printCommandMessage(StdError, server_name, stream.command_name, stream.data);
        break;
    }
}

void CFileDataSourcesReciever::writeToFile(const QString server_name, QString command_name, const QByteArray& data)
{
    if (output_files_[server_name].contains(command_name)) {
        QFile* pOutputFile = output_files_[server_name][command_name];
        if (pOutputFile) {
            pOutputFile->write(data);
            pOutputFile->flush();
        }
    }
}

void CFileDataSourcesReciever::printAppStatus(const QString& message)
{
    printServerMessage(CFileDataSourcesReciever::AppStatus, "Application", message);
}

void CFileDataSourcesReciever::printServerMessage(const CFileDataSourcesReciever::ConsoleMessageType& message_type,
                                                  const QString& server_id,
                                                  const QString& server_message)
{
    const char* pMessageTypeString = console_message_type_.valueToKey(message_type);
    printf("%12s | %-10s | %-15s | %s\n", qPrintable(currentConsoleTime()), pMessageTypeString, qPrintable(server_id), qPrintable(server_message));
}

void CFileDataSourcesReciever::printCommandMessage(const CFileDataSourcesReciever::ConsoleMessageType& message_type,
                                                   const QString& server_name,
                                                   const QString& command_name,
                                                   const QString& command_message)
{
    const char* pMessageTypeString = console_message_type_.valueToKey(message_type);
    printf("%12s | %-10s | %-15s | %-15s | %s\n", qPrintable(currentConsoleTime()), pMessageTypeString, qPrintable(server_name), qPrintable(command_name), qPrintable(command_message));
}

QString CFileDataSourcesReciever::currentConsoleTime() const
{
    return QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
}

QString CFileDataSourcesReciever::createOutputFolder(const QString& outputFolderPath) const
{
    QString result;
    if (outputFolderPath.isEmpty()) {
        result = QDateTime::currentDateTime().toString("dd-MM-yy_hh-mm-ss") + "_output";
    } else {
        result = outputFolderPath;
    }
    QDir outputFolder(result);
    result = outputFolder.absolutePath();
    if (!outputFolder.exists() &&
            !outputFolder.mkpath(result)) {
        throw std::runtime_error(QString("Cann't create output folder %1")
                                 .arg(result)
                                 .toStdString());
    }
    return result;
}

QString CFileDataSourcesReciever::getOutputFilePath(const QString& serverId, const QString& commandId, const QString& outputExtension) const
{
    return QString("%1/%2_%3.%4").arg(output_folder_path_, serverId, commandId, outputExtension);
}

void CFileDataSourcesReciever::closeOutputFile(const QString& server_name,
                                               const QString& command_name)
{
    if (output_files_[server_name].contains(command_name)) {
        QFile* output_file = output_files_[server_name][command_name];
        output_files_[server_name].remove(command_name);
        if (output_files_[server_name].isEmpty())
            output_files_.remove(server_name);
        if (output_file)
            output_file->close();
        delete output_file;
    }
}

void CFileDataSourcesReciever::createOutputFile(const QString& server_name,
                                                const QString& command_name,
                                                const QString& output_extension)
{
    if (!output_files_[server_name].contains(command_name)) {
        const QString& file_path = getOutputFilePath(server_name, command_name, output_extension);
        QFile* output_file_ptr = new QFile(file_path);
        if (!output_file_ptr->open(QIODevice::Append)) {
            qWarning() << QString("Cannot open file %1 for writing").arg(file_path);
            delete output_file_ptr;
            output_file_ptr = nullptr;
        }
        if (output_file_ptr)
            output_files_[server_name][command_name] = output_file_ptr;
    }
}
