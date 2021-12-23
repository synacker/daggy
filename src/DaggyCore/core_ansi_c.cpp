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

class Application : public QRunnable
{
public:
    void run() override {
        int argc = 1;
        const char* app_name = "ansi_c_app";
        QCoreApplication app(argc, const_cast<char**>(&app_name));
        app.exec();
    }
};

static std::uint64_t cores_count = 0;

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
        daggy::Sources parsed_sources;
        switch (text_type) {
        case Json:
            parsed_sources = std::move(*daggy::sources::convertors::json(QString(sources)));
            break;
        case Yaml:
            parsed_sources = std::move(*daggy::sources::convertors::yaml(QString(sources)));
            break;

        };

        if (cores_count == 0) {
            QThreadPool::globalInstance()->start(new Application);
        }
        cores_count++;
        *core = new daggy::Core(std::move(parsed_sources));
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
        cores_count--;
        if (cores_count == 0)
            qApp->exit();
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
        return DaggyErrorSuccess;
    });
}
