#include "Precompiled.h"
#include <DaggyCore/DaggyCore.h>
#include <DaggyCore/CSsh2DataProviderFabric.h>

#include "CFileDataAggregator.h"
#include "CConsoleDaggy.h"

using namespace daggycore;

int main(int argc, char** argv) 
try {
    QCoreApplication app(argc, argv);
    CConsoleDaggy* console_daggy = new CConsoleDaggy(&app);
    auto result = console_daggy->initialize();
    if (result)
        result = console_daggy->start();
    if (!result)
        throw std::runtime_error(result.detailed_error_message());
    return app.exec();
}
catch (const std::exception& exception) {
    std::cout << exception.what() << std::endl;
    return EXIT_FAILURE;
}
