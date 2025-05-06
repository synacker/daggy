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
#include "../Precompiled.hpp"
#include "CFile.hpp"
#include <DaggyCore/Errors.hpp>

namespace  {
const size_t max_open_files = 1000;
}


daggy::aggregators::CFile::CFile(QString output_folder,
                                 QObject* parent)
    : IAggregator(parent)
    , output_folder_(std::move(output_folder))
{
    connect(this, &CFile::nextWrite, this, &CFile::write, Qt::QueuedConnection);
}

daggy::aggregators::CFile::CFile(QObject* parent)
    : CFile(QDir::currentPath(), parent)
{

}

daggy::aggregators::CFile::~CFile()
{

}

bool daggy::aggregators::CFile::isReady() const
{
    auto aggregation_dir = QDir(output_folder_);
    return aggregation_dir.exists() || QDir().mkdir(output_folder_);
}

void daggy::aggregators::CFile::onDataProviderStateChanged(QString provider_id, DaggyProviderStates state)
{
}

void daggy::aggregators::CFile::onDataProviderError(QString provider_id, std::error_code error_code)
{

}

void daggy::aggregators::CFile::onCommandStateChanged(QString provider_id, QString command_id, DaggyCommandStates state, int exit_code)
{
}

void daggy::aggregators::CFile::onCommandError(QString provider_id, QString command_id, std::error_code error_code)
{

}

void daggy::aggregators::CFile::onCommandStream(QString provider_id, QString command_id, sources::commands::Stream stream)
{
    const auto& file_name = name(stream.meta.session, provider_id, command_id, stream.meta.type, stream.meta.extension);
    if (!streams_.contains(file_name))
    {
        auto stream_dir = QFileInfo(file_name).absoluteDir();
        if (!stream_dir.exists() && !QDir().mkpath(stream_dir.absolutePath()))
        {
            onCommandError(provider_id, command_id, errors::make_error_code(DaggyErrorStreamCorrupted));
            return;
        }
    }
    streams_[file_name].append(std::move(stream.part));
    emit nextWrite();
}

void daggy::aggregators::CFile::onDaggyStateChanged(DaggyStates state)
{

}

void daggy::aggregators::CFile::write()
{
    if (streams_.empty())
        return;
    std::list<std::unique_ptr<QFile>> files_for_flush;
    auto stream = streams_.begin();
    while (stream != streams_.end()) {
        const auto& stream_name = stream.key();
        auto& bytes_to_write = stream.value();
        std::unique_ptr<QFile> file = std::make_unique<QFile>(stream_name);
        if (file->open(QIODevice::Append)) {
            auto bytes_written = file->write(bytes_to_write);
            if (bytes_written > 0 &&
                bytes_to_write.erase(bytes_to_write.cbegin(), bytes_to_write.cbegin() + bytes_written) == bytes_to_write.end())
            {
                stream = streams_.erase(stream);
            }
            else {
                stream++;
            }
            files_for_flush.emplace_back(std::move(file));
            if (files_for_flush.size() == max_open_files)
                files_for_flush.clear();
        }
        else
        {
            stream++;
        }
    }
    if (!streams_.empty())
        emit nextWrite();
}

QString daggy::aggregators::CFile::name(const QString& session, const QString& provider_id, const QString& command_id, DaggyStreamTypes type, const QString& extension) const
{
    QString result;
    switch (type) {
    case DaggyStreamStandard:
        result = QString("%1/%2/%3-%4.%5").arg(output_folder_, session, provider_id, command_id, extension);
        break;
    case DaggyStreamError:
        result = QString("%1/%2/%3-%4.%5.%6").arg(output_folder_, session, provider_id, command_id, extension, "err");
        break;
    }
    return result;
}
