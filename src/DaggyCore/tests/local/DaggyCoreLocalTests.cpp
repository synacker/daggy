/*
MIT License

Copyright (c) 2021 Mikhail Milovidov

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

#include "Precompiled.h"
#include "DaggyCoreLocalTests.h"

#include <DaggyCore/Core.hpp>
#include <DaggyCore/CYamlDataSourcesConvertor.hpp>
#include <DaggyCore/CJsonDataSourcesConvertor.hpp>
#include <DaggyCore/CLocalDataProvidersFabric.hpp>

using namespace daggy;
using namespace daggyconv;

namespace  {
constexpr const char* json_data = R"JSON(
{
    "sources": {
        "localhost" : {
            "type": "local",
            "commands": {
                "pingpong": {
                    "exec": "pingpong",
                    "extension": "log"
                },
                "pingpong_restart": {
                    "exec": "pingpong -c 1",
                    "extension": "log",
                    "restart": true
                },
                "pingpong_once": {
                    "exec": "pingpong -c 1",
                    "extension": "log"
                }
            }
        }
    }
}
)JSON";
constexpr const char* yaml_data = R"YAML(
aliases:
    - &my_commands
        pingpong:
            exec: pingpong
            extension: log
        pingpong_restart:
            exec: pingpong -c 1
            extension: log
            restart: true
        pingpong_once:
            exec: pingpong -c 1
            extension: log
sources:
    localhost:
        type: local
        commands: *my_commands
)YAML";
const DataSources data_sources{
    {
        "localhost", {
            "localhost",
            "local",
            "",
            {
                {
                    "pingpong",
                    {
                        "pingpong",
                        "log",
                        "pingpong",
                        {},
                        false
                    }
                },
                {
                    "pingpong_restart",
                    {
                        "pingpong_restart",
                        "log",
                        "pingpong -c 1",
                        {},
                        true
                    }
                },
                {
                    "pingpong_once",
                    {
                        "pingpong_once",
                        "log",
                        "pingpong -c 1",
                        {},
                        false
                    }
                },
            },
            false,
            {}
        }
    }
};

const DataSources fake_data_sources = {
    {
        "localhost", {
            "localhost",
            "local",
            "",
            {
                {
                    "fake_ping",
                    {
                        "fake_ping",
                        "log",
                        "ping_fake 127.0.0.1",
                        {},
                        false
                    }
                }
            },
            false,
            {}
        }
    }
};

const DataSources once_process_data_sources = {
    {
        "localhost", {
            "localhost",
            "local",
            "",
            {
                {
                    "pingpong",
                    {
                        "pingpong",
                        "log",
                        "pingpong",
                        {},
                        false
                    }
                }
            },
            false,
            {}
        }
    }
};


}


DaggyCoreLocalTests::DaggyCoreLocalTests(QObject *parent)
    : QObject(parent)
    , daggy_core_(nullptr)
{

}

void DaggyCoreLocalTests::init()
{
    daggy_core_ = new Core(this);
}

void DaggyCoreLocalTests::cleanup()
{
    delete daggy_core_;
    daggy_core_ = nullptr;
}

void DaggyCoreLocalTests::checkVersion()
{
    auto version = daggy_core_->version();
    QCOMPARE(QString(version.full), QString(DAGGY_VERSION_FULL));
    QCOMPARE(version.major, DAGGY_VERSION_MAJOR);
    QCOMPARE(version.minor, DAGGY_VERSION_MINOR);
    QCOMPARE(version.patch, DAGGY_VERSION_PATCH);
    QCOMPARE(version.build, DAGGY_VERSION_BUILD);
    QCOMPARE(QString(version.postfix), QString(DAGGY_VERSION_POSTFIX));
    QCOMPARE(QString(version.vendor), QString(DAGGY_VENDOR));
}

void DaggyCoreLocalTests::startAndTerminateTest_data()
{
    QTest::addColumn<QString>("type");
    QTest::addColumn<QString>("data");

    QTest::newRow("json") << "json" << json_data;
#ifdef YAML_SUPPORT
    QTest::newRow("yaml") << "yaml" << yaml_data;
#endif
}

void DaggyCoreLocalTests::startAndTerminateTest()
{
    QFETCH(QString, type);
    QFETCH(QString, data);

    QVERIFY(daggy_core_->state() == Core::NotStarted);
    auto result = daggy_core_->setDataSources(data, type);
    QVERIFY2(result, result.detailed_error_message().c_str());
    QCOMPARE(daggy_core_->dataSources(), data_sources);

    QSignalSpy states_spy(daggy_core_, &Core::stateChanged);
    QSignalSpy streams_spy(daggy_core_, &Core::commandStream);

    QTimer::singleShot(0, [=]()
    {
        auto result = daggy_core_->start();
        QVERIFY2(result, result.detailed_error_message().c_str());
    });

    QVERIFY(states_spy.wait());
    QVERIFY(!states_spy.isEmpty());
    auto arguments = states_spy.takeFirst();
    QCOMPARE(arguments.at(0).value<Core::State>(), Core::Started);

    QTimer::singleShot(3000, [=]()
    {
        daggy_core_->stop();
    });


    QVERIFY(states_spy.wait());
    QVERIFY(!states_spy.isEmpty());
    arguments = states_spy.takeFirst();
    QCOMPARE(arguments.at(0).value<Core::State>(), Core::Finishing);

    QVERIFY(states_spy.wait());
    QVERIFY(!states_spy.isEmpty());
    arguments = states_spy.takeFirst();
    QCOMPARE(arguments.at(0).value<Core::State>(), Core::Finished);
    streams_spy.wait();
    QVERIFY(!streams_spy.isEmpty());

    QMap<QString, QList<Command::Stream>> streams;
    for (auto command_stream : streams_spy) {
        auto command_id = command_stream[1].toString();
        auto stream = command_stream[2].value<Command::Stream>();
        QVERIFY(!stream.data.isEmpty());
        streams[command_id].push_back(stream);
    }

    auto stream_keys = streams.keys();
    std::sort(stream_keys.begin(), stream_keys.end());

    QList<QString> sources_keys;
    for (const auto& command : data_sources) {
        sources_keys += command.commands.keys();
    }
    std::sort(sources_keys.begin(), sources_keys.end());

    QCOMPARE(stream_keys, sources_keys);

    QVERIFY(!streams["pingpong_once"].isEmpty());
    QVERIFY(!streams["pingpong_restart"].isEmpty());
    QVERIFY(!streams["pingpong"].isEmpty());
}

void DaggyCoreLocalTests::stopWithFakeProcess()
{
    QVERIFY(daggy_core_->state() == Core::NotStarted);
    daggy_core_->setDataSources(fake_data_sources);

    QSignalSpy states_spy(daggy_core_, &Core::stateChanged);

    QTimer::singleShot(0, [=]()
    {
        auto result = daggy_core_->start();
        QVERIFY2(result, result.detailed_error_message().c_str());
    });

    QVERIFY(states_spy.wait());
    QVERIFY(!states_spy.isEmpty());
    auto arguments = states_spy.takeFirst();
    QCOMPARE(arguments.at(0).value<Core::State>(), Core::Started);


    QVERIFY(states_spy.wait());
    QVERIFY(!states_spy.isEmpty());
    arguments = states_spy.takeFirst();
    QCOMPARE(arguments.at(0).value<Core::State>(), Core::Finished);
}

void DaggyCoreLocalTests::stopOnceProcess()
{
    QVERIFY(daggy_core_->state() == Core::NotStarted);
    daggy_core_->setDataSources(once_process_data_sources);

    QSignalSpy states_spy(daggy_core_, &Core::stateChanged);
    QSignalSpy streams_spy(daggy_core_, &Core::commandStream);

    QTimer::singleShot(0, [=]()
    {
        auto result = daggy_core_->start();
        QVERIFY2(result, result.detailed_error_message().c_str());
    });

    QVERIFY(states_spy.wait());
    QVERIFY(!states_spy.isEmpty());
    auto arguments = states_spy.takeFirst();
    QCOMPARE(arguments.at(0).value<Core::State>(), Core::Started);

    QTimer::singleShot(3000, [=]()
    {
        daggy_core_->stop();
    });

    QVERIFY(states_spy.wait());
    QVERIFY(!states_spy.isEmpty());
    QCOMPARE(states_spy.count(), 1);
    arguments = states_spy.takeFirst();
    QCOMPARE(arguments.at(0).value<Core::State>(), Core::Finishing);

    QVERIFY(states_spy.wait());
    QVERIFY(!states_spy.isEmpty());
    arguments = states_spy.takeFirst();
    QCOMPARE(arguments.at(0).value<Core::State>(), Core::Finished);
    QVERIFY(!streams_spy.isEmpty());
}
