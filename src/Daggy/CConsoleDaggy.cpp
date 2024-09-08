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
    , console_aggreagator_(nullptr)
    , need_hard_stop_(false)
{
    qApp->setApplicationName("daggy");
    qApp->setOrganizationName(DAGGY_VENDOR);
    qApp->setApplicationVersion(DAGGY_VERSION_STANDARD);
    qApp->setOrganizationDomain("https://github.com/synacker/daggy");

    connect(this, &CConsoleDaggy::interrupt, this, &CConsoleDaggy::stop, Qt::QueuedConnection);
    connect(qApp, &QCoreApplication::aboutToQuit, this, &CConsoleDaggy::fixPcaps);
}

CConsoleDaggy::~CConsoleDaggy()
{
    file_thread_.terminate();
    file_thread_.wait();
}

std::error_code CConsoleDaggy::prepare()
{
    if (daggy_core_)
        return errors::success;

    settings_ = parse();
    Sources sources;
    switch (settings_.data_source_text_type) {
    case Json:
        sources = std::move(*sources::convertors::json(settings_.data_source_text));
        break;
    case Yaml:
        sources = std::move(*sources::convertors::yaml(settings_.data_source_text));
        break;
    }

    session_ = QDateTime::currentDateTime().toString("dd-MM-yy_hh-mm-ss-zzz") + "_" + settings_.data_sources_name;

    daggy_core_ = new Core(session_, std::move(sources), this);

    connect(daggy_core_, &Core::stateChanged, this, &CConsoleDaggy::onDaggyCoreStateChanged);

    auto file_aggregator = new aggregators::CFile(settings_.output_folder);
    file_aggregator->moveToThread(&file_thread_);
    connect(this, &CConsoleDaggy::destroyed, file_aggregator, &aggregators::CFile::deleteLater);
    console_aggreagator_ = new aggregators::CConsole(session_, this);

    daggy_core_->connectAggregator(file_aggregator);
    daggy_core_->connectAggregator(console_aggreagator_);

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
        console_aggreagator_->printAppMessage("HARD STOP");
        delete daggy_core_;
        qApp->exit();
    } else {
        daggy_core_->stop();
        need_hard_stop_ = true;
    }
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

    const QCommandLineOption fix_pcap_option({"x", "fix-pcap"},
                                             "Fix and convert pcap files to pcapng");

    QCommandLineParser command_line_parser;
    command_line_parser.addOption(output_folder_option);
    command_line_parser.addOption(input_format_option);
    command_line_parser.addOption(input_from_stdin_option);
    command_line_parser.addOption(auto_complete_timeout);

#ifdef PCAPNG_SUPPORT
    command_line_parser.addOption(fix_pcap_option);
#endif

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

    if (command_line_parser.isSet(fix_pcap_option)) {
        result.fix_pcap = true;
    }

    if (command_line_parser.isSet(auto_complete_timeout)) {
        result.timeout = command_line_parser.value(auto_complete_timeout).toUInt();
    }

    if (result.output_folder.isEmpty())
        result.output_folder = QDir::currentPath();
    result.data_source_text = mustache(result.data_source_text, result.output_folder);

    return result;
}

void CConsoleDaggy::fixPcaps() const
{
    if (!settings_.fix_pcap)
        return;
#ifdef PCAPNG_SUPPORT
    auto output_folder = QDir(QDir::cleanPath(settings_.output_folder + QDir::separator() + session_));
    QDirIterator pcap_files(output_folder.absolutePath(), {"*.pcap"});
    while (pcap_files.hasNext())
    {
        const auto& pcap_file = pcap_files.next();
        const QString& pcap_name = QFileInfo(pcap_file).baseName();
        const auto& pcapng_file = QDir::cleanPath(output_folder.absolutePath() + QDir::separator() + pcap_name + ".pcapng");

        std::unique_ptr<pcpp::IFileReaderDevice> reader(pcpp::IFileReaderDevice::getReader(qPrintable(pcap_file)));

        pcpp::PcapNgFileWriterDevice pcapNgWriter(qPrintable(pcapng_file));

        if (!reader || !reader->open() || !pcapNgWriter.open())
        {
            continue;
        }

        pcpp::RawPacket rawPacket;
        while (reader->getNextPacket(rawPacket))
        {
            pcapNgWriter.writePacket(rawPacket);
        }
        reader->close();
        pcapNgWriter.close();
        output_folder.remove(pcap_file);
        console_aggreagator_->printAppMessage(QString("fix pcap %1").arg(pcap_name));
    }
#endif
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
