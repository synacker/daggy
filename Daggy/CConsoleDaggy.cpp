/*
Copyright 2017-2020 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "Precompiled.h"
#include "CConsoleDaggy.h"

#include <DaggyCore/DaggyCore.h>

#include "Common.h"

CConsoleDaggy::CConsoleDaggy(QCoreApplication* application)
    : QObject(application)
{
    application->setApplicationVersion(FULL_VERSION_STR);
    initialize();
}

void CConsoleDaggy::initialize()
{
    const auto [yaml_data_sources, output_folder] = parse();


}

bool CConsoleDaggy::handleSystemSignal(const int signal)
{
    if (signal & DEFAULT_SIGNALS) {
        emit interrupt(signal);
        return true;
    }
    return false;
}

bool CConsoleDaggy::isError() const
{
    return !error_message_.isEmpty();
}

const QString& CConsoleDaggy::errorMessage() const
{
    return error_message_;
}

std::tuple<QString, QString> CConsoleDaggy::parse() const
{
    QString yaml_data_sources;
    QString output_folder;

    const QCommandLineOption output_folder_option({"o", "output"},
                                                  "Set output folder",
                                                  "folder", "");
    const QCommandLineOption input_from_stdin_option({"i", "stdin"},
                                                     "Read data sources from stdin");

    QCommandLineParser command_line_parser;
    command_line_parser.addOption(output_folder_option);
    command_line_parser.addOption(input_from_stdin_option);
    command_line_parser.addHelpOption();
    command_line_parser.addVersionOption();
    command_line_parser.addPositionalArgument("file", "Data source file", "*.yaml");

    command_line_parser.process(*application());


    const QStringList positional_arguments = command_line_parser.positionalArguments();
    if (positional_arguments.isEmpty()) {
        command_line_parser.showHelp(-1);
    }

    const QString& source_file_name = positional_arguments.first();

    if (command_line_parser.isSet(input_from_stdin_option))
        yaml_data_sources = QTextStream(stdin).readAll();
    else {
        yaml_data_sources = getTextFromFile(source_file_name);
    }

    if (command_line_parser.isSet(output_folder_option))
        output_folder = command_line_parser.value(output_folder_option);
    else
        output_folder = generateOutputFolder(QFileInfo(source_file_name).baseName());

    return {yaml_data_sources, output_folder};
}

daggycore::DaggyCore* CConsoleDaggy::daggyCore() const
{
    return findChild<daggycore::DaggyCore*>();
}

QCoreApplication* CConsoleDaggy::application() const
{
    return qobject_cast<QCoreApplication*>(parent());
}

QString CConsoleDaggy::getTextFromFile(QString file_path) const
{
   QString result;
   if (!QFileInfo(file_path).exists()) {
       file_path = homeFolder() + file_path;
   }

   QFile source_file(file_path);
   if (source_file.open(QIODevice::ReadOnly | QIODevice::Text))
       result = source_file.readAll();
   else
       throw std::invalid_argument(QString("Cann't open %1 file for read: %2")
                                           .arg(file_path, source_file.errorString())
                                           .toStdString());

   return result;
}

QString CConsoleDaggy::generateOutputFolder(const QString& data_sources_name) const
{
    const QString current_date = QDateTime::currentDateTime().toString("dd-MM-yy_hh-mm-ss-zzz");
    return current_date + "_" + data_sources_name;
}

QString CConsoleDaggy::homeFolder() const
{
    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.daggy/";
}
