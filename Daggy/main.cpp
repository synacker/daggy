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
    console_daggy->initialize();
    console_daggy->start();

    return app.exec();
}
catch (const std::exception&) {
    return EXIT_FAILURE;
}
