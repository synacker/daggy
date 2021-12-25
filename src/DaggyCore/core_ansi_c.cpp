/*
MIT License

Copyright (c) 2020 Mikhail Milovidov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Precompiled.hpp"
#include "Core.hpp"
#include "Errors.hpp"
#include "Core.h"
#include "aggregators/CCallback.hpp"

namespace {

class ThreadFunction : public QRunnable
{
public:
    ThreadFunction(libdaggy_thread_function function,
                   void* parameter)
        : function_(function)
        , parameter_(parameter)
    {}

    void run() override {
        function_(parameter_);
    }

private:
    libdaggy_thread_function function_;
    void* parameter_;
};

std::unique_ptr<QCoreApplication> application = nullptr;

DaggyErrors safe_call(std::function<DaggyErrors ()> function)
try {
    return function();
}
catch (const std::system_error& exception)
{
    auto error = exception.code();
    if (error.category() == daggy::errors::category())
        return static_cast<DaggyErrors>(error.value());
    return DaggyErrorInternal;
}
catch (...)
{
    return DaggyErrorInternal;
}
}


DaggyErrors libdaggy_core_create(const char* sources,
                                 DaggySourcesTextTypes text_type,
                                 DaggyCore* core)
{
    return safe_call([=]()
    {
        if (!application)
            return DaggyErrorInternal;

        daggy::Sources parsed_sources;
        switch (text_type) {
        case Json:
            parsed_sources = std::move(*daggy::sources::convertors::json(QString(sources)));
            break;
        case Yaml:
            parsed_sources = std::move(*daggy::sources::convertors::yaml(QString(sources)));
            break;

        };
        auto core_object = new daggy::Core(std::move(parsed_sources));
        QObject::connect(application.get(), &QCoreApplication::aboutToQuit, core_object, &daggy::Core::stop);
        auto error = core_object->prepare();
        if (error)
        {
            throw std::system_error(std::move(error));
        }
        *core = core_object;
        return DaggyErrorSuccess;
    });
}

DaggyErrors libdaggy_core_start(DaggyCore core)
{
    return safe_call([=]()
    {
        auto core_object = static_cast<daggy::Core*>(core);
        auto error = core_object->start();
        if (error)
            throw std::system_error(std::move(error));
        return DaggyErrorSuccess;
    });
}


DaggyErrors libdaggy_core_stop(DaggyCore core)
{
    return safe_call([=]()
    {
        auto core_object = static_cast<daggy::Core*>(core);
        auto error = core_object->stop();
        if (error)
            throw std::system_error(std::move(error));
        return DaggyErrorSuccess;
    });
}

void libdaggy_core_destroy(DaggyCore* core)
{
    safe_call([=]()
    {
        auto core_object = static_cast<daggy::Core*>(*core);
        delete core_object;
        *core = nullptr;
        return DaggyErrorSuccess;
    });
}

DaggyErrors libdaggy_connect_aggregator(DaggyCore core,
                                        libdaggy_on_daggy_state_changed on_daggy_state_changed,
                                        libdaggy_on_provider_state_changed on_provider_state_changed,
                                        libdaggy_on_provider_error on_provider_error,
                                        libdaggy_on_command_state_changed on_command_state_changed,
                                        libdaggy_on_command_stream on_command_stream,
                                        libdaggy_on_command_error on_command_error)
{
    return safe_call([=](){
        auto core_object = static_cast<daggy::Core*>(core);
        auto callback_aggregator = new daggy::aggregators::CCallback(on_daggy_state_changed,
                                                                     on_provider_state_changed,
                                                                     on_provider_error,
                                                                     on_command_state_changed,
                                                                     on_command_stream,
                                                                     on_command_error,
                                                                     core_object);
        core_object->connectAggregator(callback_aggregator);
        return DaggyErrorSuccess;
    });
}

void libdaggy_app_create(int argc, char** argv)
{
    if (!application)
        application = std::make_unique<QCoreApplication>(argc, argv);
}
int libdaggy_app_exec()
{
    if (application)
    {
        auto result = application->exec();
        application.reset(nullptr);
        return result;
    }
    return std::numeric_limits<int>::min();
}
void libdaggy_app_stop()
{
    if (application)
        application->quit();
}

void libdaggy_run_in_thread(libdaggy_thread_function function, void* parameter)
{
    QThreadPool::globalInstance()->start(new ThreadFunction(function, parameter));
}
