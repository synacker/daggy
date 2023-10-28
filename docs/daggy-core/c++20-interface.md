---
description: Daggy Core C++ interface
---

# C++20 interface

## The Core object interface

### Description

The **Core** class is a QObject. It aggregates all **streams** from **providers** into once **Core Streams Session**.

The **Core** class is a main class of **Daggy Core** library and can be included by:

```cpp
#include <DaggyCore/Core.hpp>
```

### Constructor

```cpp
Core(Sources sources,
     QObject* parent = nullptr);
Core(QString session,
     Sources sources,
     QObject* parent = nullptr);
```

_Session_ is id for **Core Streams Session**. If no session was passed to constructor - new session id will generate (in uuid format).

_Sources_ is required parameter. It contains provider commands for streaming:

```cpp
namespace daggy {
namespace sources {
namespace commands {
namespace streams {
struct Meta {
    QString session;
    std::chrono::time_point<std::chrono::system_clock> start_time;
    QString extension;
    DaggyStreamTypes type;

    std::uint64_t seq_num;
    std::chrono::time_point<std::chrono::system_clock> time;
};
}

struct Stream {
    streams::Meta meta;
    QByteArray part;
};

struct DAGGYCORE_EXPORT Properties {
    QString extension;
    QString exec;
    QVariantMap parameters = {};
    bool restart = false;

    bool operator==(const Properties& other) const;
};
}

using Commands = QMap<QString, commands::Properties>;
using Command = QPair<QString, commands::Properties>;

struct DAGGYCORE_EXPORT Properties {
    QString type;
    QString host;
    Commands commands;
    bool reconnect = false;
    QVariantMap parameters = {};

    bool operator==(const Properties& other) const;
};
}

using Sources = QMap<QString, sources::Properties>;
using Source = QPair<QString, sources::Properties>;
}
```

Properties of _sources_ is described [here](../daggy-console-application/daggy-sources.md)

### Prepare and extend supported providers

```cpp
std::error_code prepare();
std::error_code prepare(std::span<providers::IFabric*> fabrics);

std::error_code prepare(QString& error) noexcept;
std::error_code prepare(std::span<providers::IFabric*> fabrics, QString& error) noexcept;
```

Before **Daggy Core** _start_, need to _prepare_ **providers**. If you need to extend provider types, that not out of box from **Daggy Core** lib (_local_ and _ssh2_), you can create own providers fabric:

```cpp
class DAGGYCORE_EXPORT IFabric
{
public:
    IFabric();
    virtual ~IFabric();

    virtual const QString& type() const = 0;
    Result<IProvider*> create(const QString& session,
                              const Source& source,
                              QObject* parent);

protected:
    virtual Result<IProvider*> createProvider(const QString& session,
                                              const Source& source,
                                              QObject* parent) = 0;
};

```

Own fabric instances need to pass in _prepare_ method.

### Connect aggregators

```cpp
std::error_code connectAggregator(aggregators::IAggregator* aggregator) noexcept;
```

Before **Daggy Core** _start_, you can add aggregators.

#### Out of box Daggy Core Aggregators

There are three types of aggregators, that already [described](../#basic-terms):

```cpp
#include <DaggyCore/aggregators/CFile.hpp>
#include <DaggyCore/aggregators/CConsole.hpp>
#include <DaggyCore/aggregators/CCallback.hpp>
```

#### User defined aggregator

User can create own aggregator by implementation of _IAggregator_ interface:

```cpp
class DAGGYCORE_EXPORT IAggregator : public QObject
{
    Q_OBJECT
public:
    IAggregator(QObject* parent = nullptr);
    virtual ~IAggregator();

    virtual bool isReady() const = 0;

public slots:
    virtual void onDataProviderStateChanged(QString provider_id,
                                            DaggyProviderStates state) = 0;
    virtual void onDataProviderError(QString provider_id,
                                     std::error_code error_code) = 0;

    virtual void onCommandStateChanged(QString provider_id,
                                       QString command_id,
                                       DaggyCommandStates state,
                                       int exit_code) = 0;

    virtual void onCommandError(QString provider_id,
                                QString command_id,
                                std::error_code error_code) = 0;

    virtual void onCommandStream(QString provider_id,
                                 QString command_id,
                                 sources::commands::Stream stream) = 0;

    virtual void onDaggyStateChanged(DaggyStates state) = 0;

};
```

### Start and stop Core

```cpp
std::error_code start() noexcept;
std::error_code stop() noexcept;
```

After _start_ is called, Core changed state by the next state machine logic:

![Daggy Core states](../.gitbook/assets/daggy\_core\_states.svg)

_Finished_ state reached by calling _stop_ method, or after all **providers** are finished.

### Streams viewing

```cpp
signals:
    void stateChanged(DaggyStates state);

    void dataProviderStateChanged(QString provider_id,
                                  DaggyProviderStates state);
    void dataProviderError(QString provider_id,
                           std::error_code error_code);

    void commandStateChanged(QString provider_id,
                             QString command_id,
                             DaggyCommandStates state,
                             int exit_code);
    void commandStream(QString provider_id,
                       QString command_id,
                       sources::commands::Stream stream);
    void commandError(QString provider_id,
                      QString command_id,
                      std::error_code error_code);
```

## Usage Example

{% code title="test.cpp" %}
```cpp
#include <DaggyCore/Core.hpp>
#include <DaggyCore/Sources.hpp>
#include <DaggyCore/aggregators/CFile.hpp>
#include <DaggyCore/aggregators/CConsole.hpp>

#include <QCoreApplication>
#include <QTimer>

namespace {
constexpr const char* json_data = R"JSON(
{
    "sources": {
        "localhost" : {
            "type": "local",
            "commands": {
                "ping1": {
                    "exec": "ping 127.0.0.1",
                    "extension": "log"
                },
                "ping2": {
                    "exec": "ping 127.0.0.1",
                    "extension": "log",
                    "restart": true
                }
            }
        }
    }
}
)JSON";
}

int main(int argc, char** argv) 
{
    QCoreApplication app(argc, argv);
    daggy::Core core(*daggy::sources::convertors::json(json_data));

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
```
{% endcode %}
