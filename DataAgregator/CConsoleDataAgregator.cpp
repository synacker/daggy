/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Precompiled.h"
#include "CConsoleDataAgregator.h"
#include "CApplicationSettings.h"

#include <DataAgregatorCore/CDataSources.h>

CConsoleDataAgregator::CConsoleDataAgregator(const CApplicationSettings& applicationSettings, QObject* pParent)
    : QObject(pParent)
    , m_fileDataSourcesReciever(applicationSettings)
    , m_dataAgregator(applicationSettings.isReconnectionMode(), applicationSettings.connectionTimeout())
{
    m_dataAgregator.addDataSourcesReciever(&m_fileDataSourcesReciever);

    connect(this, &CConsoleDataAgregator::interrupted, this, &CConsoleDataAgregator::handleInterruption);
    connect(&m_dataAgregator, &CDataAgregator::stopped, qApp, &QCoreApplication::quit);
}

void CConsoleDataAgregator::start(QStringList taskFilePath)
{
    qInfo() << "Start sessions";
    if (taskFilePath.isEmpty())
        taskFilePath << "DataSources.json";
    m_dataAgregator.start(CDataSources(getDataSourcesMap(taskFilePath)));
}

bool CConsoleDataAgregator::handleSystemSignal(const int signal)
{
    bool result = false;
    if (signal & DEFAULT_SIGNALS) {
        emit interrupted();
        result = true;
    }
    return result;
}

void CConsoleDataAgregator::handleInterruption()
{
    qInfo() << "Closing open sessions";
    m_dataAgregator.stop();
}

QVariantMap CConsoleDataAgregator::getDataSourcesMap(QString dataSourcesFilePath) const
{
    QString taskJson;
    if (!QFileInfo(dataSourcesFilePath).exists()) {
        dataSourcesFilePath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/" + dataSourcesFilePath;
    }
    QFile taskFile(dataSourcesFilePath);
    if (taskFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        taskJson = taskFile.readAll();
    } else {
        qFatal("Cann't open %s file for read: %s", qPrintable(dataSourcesFilePath), qPrintable(taskFile.errorString()));
    }

    QJsonParseError jsonParseError;
    QJsonDocument taskDocument = QJsonDocument::fromJson(taskJson.toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError)
        qFatal("Cann't parse %s json file: %s", qPrintable(dataSourcesFilePath), qPrintable(jsonParseError.errorString()));

    QVariantMap dataSourcesMap = taskDocument.toVariant().toMap();

    if (dataSourcesMap.isEmpty())
        qFatal("Cann't recognize in %s json file any data sources", qPrintable(dataSourcesFilePath));

    return dataSourcesMap;
}

QVariantMap CConsoleDataAgregator::getDataSourcesMap(const QStringList& dataSourcesFilePaths) const
{
    QVariantMap result;

    for (const QString& dataSourcesFilePath : dataSourcesFilePaths) {
        const QVariantMap& dataSourcesMap = getDataSourcesMap(dataSourcesFilePath);
        for (const QString& serverId : dataSourcesMap.keys()) {
            result[serverId] = dataSourcesMap[serverId];
        }
    }

    return result;
}
