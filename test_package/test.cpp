#include <DaggyCore/Core.hpp>
#include <DaggyCore/Sources.hpp>
#include <DaggyCore/aggregators/CFile.hpp>
#include <DaggyCore/aggregators/CConsole.hpp>

int main() 
{
    daggy::Sources sources;
    daggy::Core core(std::move(sources));

    daggy::aggregators::CFile file_aggregator("test");
    daggy::aggregators::CConsole console_aggregator("test");

    core.connectAggregator(&file_aggregator);
    core.connectAggregator(&console_aggregator);

    return 0;
}