#include "Precompiled.hpp"
#include "CConsoleDaggy.hpp"

using namespace daggy;

int main(int argc, char** argv) 
try {
    QCoreApplication app(argc, argv);
    auto* console_daggy = new CConsoleDaggy(&app);
    auto error = console_daggy->prepare();
    if (error)
        throw std::system_error(error);

    error = console_daggy->start();
    if (error)
        throw std::system_error(error);

    return app.exec();
}
catch (const std::exception& exception) {
    std::cout << "Runtime error - " << exception.what() << std::endl;
    return EXIT_FAILURE;
}
