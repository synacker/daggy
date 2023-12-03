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
#include "Precompiled.hpp"
#include "CConsoleDaggy.hpp"

#include <DaggyCore/Core.hpp>
#include <DaggyCore/aggregators/CConsole.hpp>
#include <DaggyCore/aggregators/CFile.hpp>
#include <DaggyCore/Types.hpp>
#include <DaggyCore/Errors.hpp>
#include <DaggyCore/version.h>

using namespace daggy;

CConsoleDaggy::CConsoleDaggy(QObject* parent)
    : QObject(parent)
    , daggy_core_(nullptr)
    , need_hard_stop_(false)
{
    qApp->setApplicationName("daggy");
    qApp->setApplicationVersion(DAGGY_VERSION_STANDARD);
    qApp->setOrganizationName(DAGGY_VENDOR);

    connect(this, &CConsoleDaggy::interrupt, this, &CConsoleDaggy::stop, Qt::QueuedConnection);
}

std::error_code CConsoleDaggy::prepare()
{
    if (daggy_core_)
        return errors::success;
    const auto settings = parse();
    Sources sources;
    switch (settings.data_source_text_type) {
    case Json:
        sources = std::move(*sources::convertors::json(settings.data_source_text));
        break;
    case Yaml:
        sources = std::move(*sources::convertors::yaml(settings.data_source_text));
        break;
    }

    const QString& session = QDateTime::currentDateTime().toString("dd-MM-yy_hh-mm-ss-zzz") + "_" + settings.data_sources_name;

    daggy_core_ = new Core(session, std::move(sources), this);

    connect(daggy_core_, &Core::stateChanged, this, &CConsoleDaggy::onDaggyCoreStateChanged);

    auto file_aggregator = new aggregators::CFile(settings.output_folder);
    file_aggregator->moveToThread(&file_thread_);
    connect(this, &CConsoleDaggy::destroyed, file_aggregator, &aggregators::CFile::deleteLater);
    auto console_aggregator = new aggregators::CConsole(session, daggy_core_);

    daggy_core_->connectAggregator(file_aggregator);
    daggy_core_->connectAggregator(console_aggregator);

    return daggy_core_->prepare();;
}

std::error_code CConsoleDaggy::start()
{
    file_thread_.start();
    return daggy_core_->start();
}

void CConsoleDaggy::stop()
{
    if (need_hard_stop_) {
        qWarning() << "HARD STOP";
        delete daggy_core_;
        qApp->exit();
    } else {
        daggy_core_->stop();
        need_hard_stop_ = true;
    }
    file_thread_.quit();
    file_thread_.wait();
}

bool CConsoleDaggy::handleSystemSignal(const int signal)
{
    if (signal & DEFAULT_SIGNALS) {
        emit interrupt(signal);
        return true;
    }
    return false;
}

const QVector<QString>& CConsoleDaggy::supportedConvertors() const
{
    static thread_local QVector<QString> formats = {
        "json",
#ifdef YAML_SUPPORT
        "yaml"
#endif
    };
    return formats;
}

DaggySourcesTextTypes CConsoleDaggy::textFormatType(const QString& file_name) const
{
    DaggySourcesTextTypes result = Json;
    const QString& extension = QFileInfo(file_name).suffix();
    if (extension == "yml" || extension == "yaml")
        result = Yaml;
    return result;
}

bool CConsoleDaggy::isError() const
{
    return !error_message_.isEmpty();
}

const QString& CConsoleDaggy::errorMessage() const
{
    return error_message_;
}

CConsoleDaggy::Settings CConsoleDaggy::parse() const
{
    Settings result;

    const QCommandLineOption output_folder_option({"o", "output"},
                                                  "Set output folder",
                                                  "folder", "");
    const QCommandLineOption input_format_option({"f", "format"},
                                                 "Source format",
                                                 supportedConvertors().join("|"),
                                                 supportedConvertors()[0]);
    const QCommandLineOption auto_complete_timeout({"t", "timeout"},
                                                   "Auto complete timeout",
                                                   "time in ms",
                                                   0
                                                   );
    const QCommandLineOption input_from_stdin_option({"i", "stdin"},
                                                     "Read data aggregation sources from stdin");

    QCommandLineParser command_line_parser;
    command_line_parser.addOption(output_folder_option);
    command_line_parser.addOption(input_format_option);
    command_line_parser.addOption(input_from_stdin_option);
    command_line_parser.addOption(auto_complete_timeout);
    command_line_parser.addHelpOption();
    command_line_parser.addVersionOption();
    command_line_parser.addPositionalArgument("file", "data aggregation sources file", "*.yaml|*.yml|*.json");

    command_line_parser.process(*qApp);


    const QStringList positional_arguments = command_line_parser.positionalArguments();
    if (positional_arguments.isEmpty()) {
        command_line_parser.showHelp(-1);
    }

    const QString& source_file_name = positional_arguments.first();

    if (command_line_parser.isSet(input_from_stdin_option)) {
        result.data_source_text = QTextStream(stdin).readAll();
        result.data_sources_name = "stdin";
    }
    else {
        result.data_source_text = getTextFromFile(source_file_name);
        result.data_sources_name = QFileInfo(source_file_name).baseName();
    }

    if (command_line_parser.isSet(output_folder_option))
        result.output_folder = command_line_parser.value(output_folder_option);
    else
        result.output_folder = QString();

    if (command_line_parser.isSet(input_format_option)) {
        const QString& format = command_line_parser.value(input_format_option);
        if (!supportedConvertors().contains(format.toLower()))
        {
            command_line_parser.showHelp(-1);
        } else {
            result.data_source_text_type = textFormatType(format);
        }
    } else {
        result.data_source_text_type = textFormatType(source_file_name);
    }

    if (command_line_parser.isSet(auto_complete_timeout)) {
        result.timeout = command_line_parser.value(auto_complete_timeout).toUInt();
    }

    if (result.output_folder.isEmpty())
        result.output_folder = QDir::currentPath();
    result.data_source_text = mustache(result.data_source_text, result.output_folder);

    return result;
}

daggy::Core* CConsoleDaggy::daggyCore() const
{
    return findChild<daggy::Core*>();
}

QCoreApplication* CConsoleDaggy::application() const
{
    return qobject_cast<QCoreApplication*>(parent());
}

QString CConsoleDaggy::getTextFromFile(QString file_path) const
{
   QString result;
   if (!QFileInfo::exists(file_path)) {
       file_path = homeFolder() + file_path;
   }

   QFile source_file(file_path);
   if (source_file.open(QIODevice::ReadOnly | QIODevice::Text))
       result = source_file.readAll();
   else
       throw std::invalid_argument(QString("Cann't open %1 file for read: %2")
                                           .arg(file_path, source_file.errorString())
                                           .toStdString());
   if (result.isEmpty())
       throw std::invalid_argument(QString("%1 file is empty")
                                           .arg(file_path)
                                           .toStdString());

   return result;
}

QString CConsoleDaggy::homeFolder() const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

QString CConsoleDaggy::mustache(const QString& text, const QString& output_folder) const
{
    QProcessEnvironment process_environment = QProcessEnvironment::systemEnvironment();
    kainjow::mustache::mustache tmpl(qUtf8Printable(text));
    kainjow::mustache::data variables;
    for (const auto& key : process_environment.keys()) {
        variables.set(qPrintable(QString("env_%1").arg(key)),
                      qUtf8Printable(process_environment.value(key)));
    }
    variables.set("output_folder", qUtf8Printable(output_folder));
    return QString::fromStdString(tmpl.render(variables));
}

void CConsoleDaggy::onDaggyCoreStateChanged(DaggyStates state)
{
    switch (state) {
    case DaggyNotStarted:
        break;
    case DaggyStarted:
        break;
    case DaggyFinishing:
        break;
    case DaggyFinished:
        qApp->exit();
        break;
    }
}
