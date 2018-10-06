/*
Copyright 2017-2018 Mikhail Milovidov <milovidovmikhail@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Precompiled.h"
#include "CApplicationSettings.h"

using namespace dataagregatorcore;

CApplicationSettings::CApplicationSettings()
{
    qApp->setApplicationVersion(DATAAGREGATOR_VERSION);
    qApp->setApplicationName(PROGRAM_NAME);

    QCommandLineParser command_line_parser;
    command_line_parser.addVersionOption();
    const QCommandLineOption output_folder_option({"o", "output"}, "Set output folder", "folder", "");
    const QCommandLineOption input_stream_option({"i", "input"}, "Input stream");

    command_line_parser.addOption(output_folder_option);
    command_line_parser.addOption(input_stream_option);

    command_line_parser.setApplicationDescription("Data agregation utility is a program for extracting information from multiple remote servers via ssh simultaneously.");
    const QCommandLineOption helpOption = command_line_parser.addHelpOption();

    command_line_parser.addPositionalArgument("sources", "Data sources", "sourceFile1 [sourceFile2 ...]");

    command_line_parser.process(*qApp);

    input_stream_ = command_line_parser.isSet(input_stream_option);

    data_sources_files_ = command_line_parser.positionalArguments();

    output_folder_ = command_line_parser.value(output_folder_option);

    if (output_folder_.isEmpty())
        output_folder_ = getOutputFolderPath(data_sources_files_);
}

const QStringList& CApplicationSettings::dataSourcesFiles() const
{
    return data_sources_files_;
}

const QString& CApplicationSettings::outputFolder() const
{
    return output_folder_;
}

DataSources CApplicationSettings::dataSources() const
{
    DataSources result;
    if (input_stream_) {
        QTextStream text_stream(stdin);
        result = DataSource::convertDataSources(getDataSourcesFromText(text_stream.readAll()));
    } else {
        result = DataSource::convertDataSources(getDataSourcesMap(data_sources_files_));
    }
    return result;
}

QString CApplicationSettings::getOutputFolderPath(const QStringList& dataSourcesFiles) const
{
    const QString& currentDate = QDateTime::currentDateTime().toString("dd-MM-yy_hh-mm-ss");
    QString dataSourcesEnvironmentName;
    for (const QString& dataSourcesFilePath : dataSourcesFiles) {
        dataSourcesEnvironmentName += "-" + QFileInfo(dataSourcesFilePath).baseName();
    }
    return currentDate + dataSourcesEnvironmentName;
}

QVariantMap CApplicationSettings::getDataSourcesMap(const QStringList& data_sources_file_paths) const
{
    QVariantMap result;

    for (const QString& dataSources_file_path : data_sources_file_paths) {
        const QVariantMap& data_sources_map = getDataSourcesMap(dataSources_file_path);
        for (const QString& serverId : data_sources_map.keys()) {
            result[serverId] = data_sources_map[serverId];
        }
    }

    return result;
}


QVariantMap CApplicationSettings::getDataSourcesFromText(const QString& sources_json) const
{
    QJsonParseError json_parse_error;
    QJsonDocument task_document = QJsonDocument::fromJson(sources_json.toUtf8(), &json_parse_error);

    if (json_parse_error.error != QJsonParseError::NoError)
        qFatal("Cann't parse json: %s", qPrintable(json_parse_error.errorString()));

    QVariantMap data_sources_map = task_document.toVariant().toMap();

    if (data_sources_map.isEmpty())
        qFatal("Cann't recognize json");
    return data_sources_map;
}

QVariantMap CApplicationSettings::getDataSourcesMap(QString data_sources_file_path) const
{
    QString sources_json;
    if (!QFileInfo(data_sources_file_path).exists()) {
        data_sources_file_path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/" + data_sources_file_path;
    }
    QFile sources_file(data_sources_file_path);
    if (sources_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        sources_json = sources_file.readAll();
    } else {
        qFatal("Cann't open %s file for read: %s", qPrintable(data_sources_file_path), qPrintable(sources_file.errorString()));
    }

    return getDataSourcesFromText(sources_json);;
}
