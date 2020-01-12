#include "Precompiled.h"
#include <DaggyCore/DaggyCore.h>
#include <DaggyCore/CSsh2DataProviderFabric.h>

#include "CFileDataAggregator.h"
#include "CConsoleDaggy.h"

using namespace daggycore;


int main(int argc, char** argv) 
{
    QCoreApplication app(argc, argv);
    CConsoleDaggy* console_daggy = new CConsoleDaggy(&app);


    /*CSsh2DataProviderFabric* ssh2_fabric = new CSsh2DataProviderFabric;
    CFileDataAggregator* file_data_aggregator = new CFileDataAggregator;

    DataSources data_sources;
    DataSource data_source;
    data_source.id = "localhost";
    data_source.host = QHostAddress("127.0.0.1");
    data_source.type = "ssh2";
    data_source.reconnect = false;

    Command command;
    command.id = "journalctl";
    command.exec = "journalctl -f";
    command.restart = false;
    command.extension = "log";

    Command command2;
    command2.id = "tail";
    command2.exec = "cat /home/muxa/.ssh/id_rsa";
    command2.restart = false;
    command2.extension = "log";

    data_source.commands[command.id] = command;
    data_source.commands[command2.id] = command2;

    data_sources[data_source.id] = data_source;

    DaggyCore* daggy_core = new DaggyCore({ssh2_fabric},
                                          {file_data_aggregator},
                                          data_sources,
                                          &app);
    QObject::connect(daggy_core, &DaggyCore::stateChanged,
                    [&app](DaggyCore::State state)
    {
        qDebug() << "Daggy core state changed: " << state;
        if (state == DaggyCore::Finished)
            app.exit();
    });

    if (!daggy_core->start()) {
        qDebug() << daggy_core->errorCode().message().c_str() << " - " << daggy_core->errorMessage();
        return -1;
    }*/

    return app.exec();
}
