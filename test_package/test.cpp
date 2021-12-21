#include <DaggyCore/Core.hpp>
#include <DaggyCore/Sources.hpp>
#include <DaggyCore/aggregators/CFile.hpp>
#include <DaggyCore/aggregators/CConsole.hpp>

#include <QCoreApplication>
#include <QTimer>

namespace {
constexpr const char* yaml_data = R"YAML(
aliases:
    - &my_commands
        ping1:
            exec: ping 127.0.0.1
            extension: log
        ping2:
            exec: ping 127.0.0.1
            extension: log
sources:
    localhost:
        type: local
        commands: *my_commands
)YAML";
}

int main(int argc, char** argv) 
{
    QCoreApplication app(argc, argv);
    daggy::Core core(*daggy::sources::convertors::yaml(yaml_data));

    daggy::aggregators::CFile file_aggregator("test");
    daggy::aggregators::CConsole console_aggregator("test");

    core.connectAggregator(&file_aggregator);
    core.connectAggregator(&console_aggregator);

    QObject::connect(&core, &daggy::Core::stateChanged, &core,
    [&](DaggyStates state){
        if(state == DaggyFinished)
            app.quit();      
    });

    QTimer::singleShot(3000, &core, [&]()
    {
        core.stop();
    });

    QTimer::singleShot(5000, &core, [&]()
    {
        app.exit(-1);
    });

    core.prepare();
    core.start();

    return app.exec();
}