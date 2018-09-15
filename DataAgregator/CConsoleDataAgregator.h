/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CCONSOLEDATAAGREGATOR_H
#define CCONSOLEDATAAGREGATOR_H

#include <QObject>
#include "ISystemSignalsHandler.h"

#include "CFileDataSourcesReciever.h"

#include <QStringList>
#include <QVariantMap>

#include <DataAgregatorCore/CDataAgregator.h>

class CApplicationSettings;


class CConsoleDataAgregator : public QObject, public ISystemSignalHandler
{
  Q_OBJECT
public:
  CConsoleDataAgregator(const CApplicationSettings& applicationSettings, QObject* pParent = nullptr);

  void start(QStringList taskFilePath);

signals:
  void interrupted();

protected:
  bool handleSystemSignal(const int signal) override;

private slots:
  void handleInterruption();

private:
  QVariantMap getDataSourcesMap(const QStringList& dataSourcesFilePaths) const;

  CFileDataSourcesReciever m_fileDataSourcesReciever;
  CDataAgregator m_dataAgregator;
  QVariantMap getDataSourcesMap(QString dataSourcesFilePath) const;
};

#endif // CCONSOLEDATAAGREGATOR_H
