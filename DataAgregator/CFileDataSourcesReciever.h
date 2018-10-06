/*
Copyright 2017-2018 Mikhail Milovidov <milovidovmikhail@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CFILEDATASOURCESRECIEVER_H
#define CFILEDATASOURCESRECIEVER_H

#include <QObject>
#include <QMap>
#include <QMetaEnum>

#include <DataAgregatorCore/IRemoteAgregatorReciever.h>

class CApplicationSettings;
class QFile;

class CFileDataSourcesReciever : public dataagregatorcore::IRemoteAgregatorReciever
{
  Q_OBJECT
  Q_ENUMS(ConsoleMessageType)
public:
  enum ConsoleMessageType {StdError, ConnStatus, CommStatus};

  CFileDataSourcesReciever(const QString& output_folder,
                           QObject* parent_ptr = nullptr);
  virtual ~CFileDataSourcesReciever() override;

private slots:
  void onConnectionStatusChanged(const QString server_name,
                                 const dataagregatorcore::RemoteConnectionStatus status,
                                 const QString message) override final;
  void onRemoteCommandStatusChanged(const QString server_name,
                                    const dataagregatorcore::RemoteCommand remote_command,
                                    const dataagregatorcore::RemoteCommand::Status status,
                                    const int exit_code) override final;
  void onNewRemoteCommandStream(const QString server_name,
                                const dataagregatorcore::RemoteCommand::Stream stream) override final;


private:
  void writeToFile(const QString server_name, QString command_name, const QByteArray& data);
  void printServerMessage(const ConsoleMessageType& message_type, const QString& server_id, const QString& server_message);
  void printCommandMessage(const ConsoleMessageType& message_type, const QString& server_name, const QString& command_name, const QString& command_message);
  QString currentConsoleTime() const;

  QString createOutputFolder(const QString& outputFolderPath) const;
  QString getOutputFilePath(const QString& serverId, const QString& commandId, const QString& outputExtension) const;
  void createOutputFile(const QString& server_name, const QString& command_name, const QString& output_extension);
  void closeOutputFile(const QString& server_name, const QString& command_name);

  const QString output_folder_path_;
  QMap<QString, QMap<QString, QFile*>> output_files_;
  QMetaEnum console_message_type_;

  // IRemoteAgregatorReciever interface
};

#endif // CFILEDATASOURCESRECIEVER_H
