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
#include "Precompiled.h"
#include "CFileDataAggregator.h"

#include <DaggyCore/IDataProvider.h>
#include <DaggyCore/Common.h>

CFileDataAggregator::CFileDataAggregator
(
        QString output_folder,
        QString data_sources_name,
        QObject* parent
)
    : daggycore::IDataAggregator(parent)
    , output_folder_(std::move(output_folder))
    , data_sources_name_(std::move(data_sources_name))
    , console_message_type_(QMetaEnum::fromType<ConsoleMessageType>())
    , provider_state_(QMetaEnum::fromType<daggycore::IDataProvider::State>())
    , command_state_(QMetaEnum::fromType<daggycore::Command::State>())
{
}

CFileDataAggregator::~CFileDataAggregator()
{
}

daggycore::Result CFileDataAggregator::prepare()
{
    if (output_folder_.isEmpty())
        current_folder_ = generateOutputFolder(data_sources_name_);
    else
        current_folder_ = output_folder_;
    auto aggregation_dir = QDir(current_folder_);
    if (!aggregation_dir.exists() && !QDir().mkdir(current_folder_))
        return
        {
            daggycore::DaggyErrors::CannotPrepareAggregator
        };

    printAppMessage(QString("Start aggregation in %1").arg(current_folder_));
    return daggycore::Result::success;
}

daggycore::Result CFileDataAggregator::free()
{
    stream_files_.clear();
    printAppMessage(QString("Stop aggregation in %1").arg(current_folder_));
    return daggycore::Result::success;
}

void CFileDataAggregator::onDataProviderStateChanged(const QString provider_id,
                                                     const int state)
{
    const auto provider_state = static_cast<daggycore::IDataProvider::State>(state);
    const char* provider_state_string = provider_state_.valueToKey(provider_state);
    const QString message = QString("New state: %1").arg(provider_state_string);
    printProviderMessage
    (
        ConsoleMessageType::ProvStat,
        provider_id,
        message
    );
}

void CFileDataAggregator::onDataProviderError(const QString provider_id,
                                              const std::error_code error_code)
{
    printProviderMessage
    (
        ProvError,
        provider_id,
        QString::fromStdString(error_code.message())
    );
}

void CFileDataAggregator::onCommandStateChanged(const QString provider_id,
                                                const QString command_id,
                                                const daggycore::Command::State state,
                                                const int exit_code)
{
    const char* state_string = command_state_.valueToKey(state);
    QString message = QString("New state: %1").arg(state_string);
    if (state == daggycore::Command::Finished)
        message += QString(". Exit code: %1").arg(exit_code);
    printCommandMessage
    (
        CommStat,
        provider_id,
        command_id,
        message
    );
}

void CFileDataAggregator::onCommandStream(const QString provider_id,
                                          const QString command_id,
                                          const daggycore::Command::Stream stream)
{
    switch (stream.type) {
        case daggycore::Command::Stream::Type::Standard:
            if (!writeToFile(provider_id,
                             command_id,
                             stream))
            {
                printCommandMessage(CommError,
                                    provider_id,
                                    command_id,
                                    "Cannot write data");
            }
            break;
        case daggycore::Command::Stream::Type::Error:
            printCommandMessage(CommError,
                                provider_id,
                                command_id,
                                stream.data);
            break;
    }
}

void CFileDataAggregator::onCommandError(const QString provider_id,
                                         const QString command_id,
                                         const std::error_code error_code)
{
    printCommandMessage
    (
        CommStat,
        provider_id,
        command_id,
        QString::fromStdString(error_code.message())
    );
}

void CFileDataAggregator::printAppMessage(const QString& message)
{
    const char* message_type_string = console_message_type_.valueToKey(AppStat);
    printf
    (
           "%12s | %-8s | %s\n",
           qPrintable(currentConsoleTime()),
           message_type_string,
           qPrintable(message)
    );
}

void CFileDataAggregator::printProviderMessage
(
        const CFileDataAggregator::ConsoleMessageType& message_type,
        const QString& provider_id,
        const QString& source_message
)
{
    const char* message_type_string = console_message_type_.valueToKey(message_type);
    printf
    (
           "%12s | %-8s | %-15s | %s\n",
           qPrintable(currentConsoleTime()),
           message_type_string,
           qPrintable(provider_id),
           qPrintable(source_message)
    );
}

void CFileDataAggregator::printCommandMessage
(
    const CFileDataAggregator::ConsoleMessageType& message_type,
    const QString& provider_id,
    const QString& command_id,
    const QString& command_message)
{
    const char* message_type_string = console_message_type_.valueToKey(message_type);
    printf
    (
           "%12s | %-8s | %-15s | %-15s | %s\n",
           qPrintable(currentConsoleTime()),
           message_type_string,
           qPrintable(provider_id),
           qPrintable(command_id),
           qPrintable(command_message)
    );
}

QString CFileDataAggregator::currentConsoleTime() const
{
    return QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
}

QString CFileDataAggregator::generateOutputFolder(const QString& data_sources_name) const
{
    const QString current_date = QDateTime::currentDateTime().toString("dd-MM-yy_hh-mm-ss-zzz");
    return current_date + "_" + data_sources_name;
}

bool CFileDataAggregator::writeToFile(const QString& provider_id,
    const QString& command_id,
    const daggycore::Command::Stream& stream)
{
    const QString file_name = QString("%1-%2.%3").arg(provider_id, command_id, stream.extension);
    QFile* file = nullptr;
    if (!stream_files_.contains(file_name)) {
        file = new QFile(QString("%1/%2").arg(current_folder_, file_name));
        if (file->open(QIODevice::Append)) {
            stream_files_.insert(file_name, std::shared_ptr<QFile>(file));
        } else
            return false;
    } else {
        file = stream_files_[file_name].get();
    }
    qint64 start = 0;
    const QByteArray& data = stream.data;
    while(start < data.size()) {
        start += file->write(data.data() + start, data.size());
    }
    file->flush();
    return true;
}
