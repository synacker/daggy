/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Precompiled.h"
#include "CConsoleDataAgregator.h"
#include "CApplicationSettings.h"

#include <DataAgregatorCore/CDataAgregator.h>

using namespace dataagregatorcore;

CConsoleDataAgregator::CConsoleDataAgregator(const DataSources& data_sources,
                                             const QString& output_folder,
                                             QObject* parent_ptr)
    : QObject(parent_ptr)
    , file_remote_agregator_reciever_(output_folder)
    , data_agregator_(data_sources)
    , stopped_(false)
{
    data_agregator_.connectRemoteAgregatorReciever(&file_remote_agregator_reciever_);

    connect(this, &CConsoleDataAgregator::interrupted, this, &CConsoleDataAgregator::handleInterruption);
    connect(&data_agregator_, &CDataAgregator::stateChanged, this, &CConsoleDataAgregator::onDataAgregatorStateChange);
}

void CConsoleDataAgregator::start()
{
    qInfo() << "Start sessions";
    data_agregator_.start();
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
    data_agregator_.stop();
}

void CConsoleDataAgregator::onDataAgregatorStateChange(const IRemoteAgregator::State state)
{
    if (state == IRemoteAgregator::State::Stopped) {
        stopped_ = true;
        qApp->quit();
    }
}

bool CConsoleDataAgregator::stopped() const
{
    return stopped_;
}
