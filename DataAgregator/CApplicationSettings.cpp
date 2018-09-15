/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Precompiled.h"
#include "CApplicationSettings.h"

CApplicationSettings::CApplicationSettings(QCoreApplication *pApplication)
    : m_connectionTimeout(2)
{
    pApplication->setApplicationVersion(DATAAGREGATOR_VERSION);
    pApplication->setApplicationName(PROGRAM_NAME);

    QCommandLineParser commandLineParser;
    commandLineParser.addVersionOption();
    const QCommandLineOption outputFolderOption({"o", "output"}, "Set output folder", "folder", "");
    const QCommandLineOption reconnectionModeOption({"r", "reconnect"}, "Enable reconnection mode");
    const QCommandLineOption connectionTimeoutOption({"t", "timeout"}, "Connection timeout", "seconds", QString::number(m_connectionTimeout));

    commandLineParser.addOption(outputFolderOption);
    commandLineParser.addOption(reconnectionModeOption);
    commandLineParser.addOption(connectionTimeoutOption);

    commandLineParser.setApplicationDescription("Data agregation utility is a program for extracting information from multiple remote servers via ssh simultaneously.");
    const QCommandLineOption helpOption = commandLineParser.addHelpOption();

    commandLineParser.addPositionalArgument("sources", "Data sources", "sourceFile1 [sourceFile2 ...]");

    commandLineParser.process(*pApplication);

    m_dataSourcesFiles = commandLineParser.positionalArguments();

    m_outputFolder = commandLineParser.value(outputFolderOption);
    m_isReconnectionMode = commandLineParser.isSet(reconnectionModeOption);

    m_connectionTimeout = commandLineParser.value(connectionTimeoutOption).toInt();

    if (m_outputFolder.isEmpty())
      m_outputFolder = getOutputFolderPath(m_dataSourcesFiles);
}

const QStringList& CApplicationSettings::dataSourcesFiles() const
{
    return m_dataSourcesFiles;
}

const QString& CApplicationSettings::outputFolder() const
{
    return m_outputFolder;
}

bool CApplicationSettings::isReconnectionMode() const
{
    return m_isReconnectionMode;
}

int CApplicationSettings::connectionTimeout() const
{
    return m_connectionTimeout;
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
