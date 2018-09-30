/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Precompiled.h"
#include "CApplicationSettings.h"

CApplicationSettings::CApplicationSettings()
{
    qApp->setApplicationVersion(DATAAGREGATOR_VERSION);
    qApp->setApplicationName(PROGRAM_NAME);

    QCommandLineParser commandLineParser;
    commandLineParser.addVersionOption();
    const QCommandLineOption outputFolderOption({"o", "output"}, "Set output folder", "folder", "");

    commandLineParser.addOption(outputFolderOption);

    commandLineParser.setApplicationDescription("Data agregation utility is a program for extracting information from multiple remote servers via ssh simultaneously.");
    const QCommandLineOption helpOption = commandLineParser.addHelpOption();

    commandLineParser.addPositionalArgument("sources", "Data sources", "sourceFile1 [sourceFile2 ...]");

    commandLineParser.process(*qApp);

    data_sources_files_ = commandLineParser.positionalArguments();

    output_folder_ = commandLineParser.value(outputFolderOption);

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
    return DataSource::convertDataSources(getDataSourcesMap(data_sources_files_));
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


QVariantMap CApplicationSettings::getDataSourcesMap(QString data_sources_file_path) const
{
    QString task_json;
    if (!QFileInfo(data_sources_file_path).exists()) {
        data_sources_file_path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/" + data_sources_file_path;
    }
    QFile task_file(data_sources_file_path);
    if (task_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        task_json = task_file.readAll();
    } else {
        qFatal("Cann't open %s file for read: %s", qPrintable(data_sources_file_path), qPrintable(task_file.errorString()));
    }

    QJsonParseError json_parse_error;
    QJsonDocument task_document = QJsonDocument::fromJson(task_json.toUtf8(), &json_parse_error);

    if (json_parse_error.error != QJsonParseError::NoError)
        qFatal("Cann't parse %s json file: %s", qPrintable(data_sources_file_path), qPrintable(json_parse_error.errorString()));

    QVariantMap data_sources_map = task_document.toVariant().toMap();

    if (data_sources_map.isEmpty())
        qFatal("Cann't recognize in %s json file any data sources", qPrintable(data_sources_file_path));

    return data_sources_map;
}
