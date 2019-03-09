/*
Copyright 2017-2018 Mikhail Milovidov <milovidovmikhail@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Precompiled.h"
#include "CApplicationSettings.h"
#include <DataAgregatorCore/CDataSourcesFabric.h>

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
    if (input_stream_)
        data_source_name_ = "stdin";
    else {
        const QStringList& positional_arguments = command_line_parser.positionalArguments();
        if (!positional_arguments.isEmpty())
            data_source_name_ = positional_arguments[0];
        else {
           qFatal("You must specify source file name or use -i flag for getting source from stdin stream");
        }
    }

    output_folder_ = command_line_parser.value(output_folder_option);

    if (output_folder_.isEmpty())
        output_folder_ = getOutputFolderPath();
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
        result = CDataSourcesFabric::getDataSources(CDataSourcesFabric::Json, text_stream.readAll());
    } else {
        result = CDataSourcesFabric::getDataSources(CDataSourcesFabric::Json, getTextFromFile(data_source_name_));
    }
    return result;
}

QString CApplicationSettings::getOutputFolderPath() const
{
    const QString& current_date = QDateTime::currentDateTime().toString("dd-MM-yy_hh-mm-ss");
    QString result = current_date + "-" + QFileInfo(data_source_name_).baseName();
    return result;
}

QString CApplicationSettings::getTextFromFile(QString file_path) const
{
    QString result;
    if (!QFileInfo(file_path).exists()) {
        file_path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/" + file_path;
    }

    QFile source_file(file_path);
    if (source_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        result = source_file.readAll();
    } else {
        qFatal("Cann't open %s file for read: %s", qPrintable(file_path), qPrintable(source_file.errorString()));
    }

    return result;
}
