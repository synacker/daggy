---
description: AI Agent Context for Daggy Project
---

# AGENTS.md

**Daggy** - Data Aggregation Utility. Server-less cross-platform tool for running multiple local/remote processes simultaneously and streaming aggregated output.

## Project Structure

```
src/
  Daggy/              # Console application (CLI)
    main.cpp          # Entry point
    CConsoleDaggy.cpp # Main console logic
  DaggyCore/          # Core library (C++23)
    Core.cpp/hpp      # Main API
    Sources.cpp/hpp   # Configuration types
    Types.h/hpp       # Enums (DaggyProviderStates, DaggyCommandStates, DaggyStates)
    Errors.cpp/hpp    # Error handling
    providers/        # Data source agents (Local, SSH, SSH2)
    aggregators/      # Data consumer agents (Console, File, Callback)
build/                # CMake build outputs
docs/                 # Documentation (markdown)
CI/conan/             # Conan package configs
test_package/         # Integration tests
```

## Tech Stack

- **Language**: C++23 with Qt6 (QObject, signals/slots)
- **Build**: CMake 3.20+
- **Package**: Conan 2.0+
- **SSH**: libssh2, OpenSSL
- **Config**: YAML (libyaml) and JSON (nlohmann/json)
- **Tests**: CTest (CMake)

## Core Architecture

### Providers (Data Sources)

| Type | Class | Purpose |
|------|-------|---------|
| `local` | `CLocal` | Execute local processes |
| `ssh` | `CSsh` | SSH v1/2 connections (pure SSH) |
| `ssh2` | `CSsh2` | SSH via libssh2 |

All providers implement `IProvider` interface with signals: `stateChanged`, `commandStateChanged`, `commandStream`, `commandError`, `error`.

### Aggregators (Data Consumers)

| Type | Class | Purpose |
|------|-------|---------|
| Console | `CConsole` | Stream to stdout |
| File | `CFile` | Write to output files |
| Callback | `CCallback` | Custom C++ callbacks |

All aggregators implement `IAggregator` interface with slots: `onDataProviderStateChanged`, `onCommandStateChanged`, `onCommandStream`, `onCommandError`, `onDaggyStateChanged`.

### State Enums (Types.h)

```cpp
enum class DaggyProviderStates { Created, Running, Stopped, Completed };
enum class DaggyCommandStates { Queued, Running, Stopped, Completed, Error };
enum class DaggyStates { Created, Aggregating, Stopped, Completed };
enum class DaggyStreamTypes { Stdout, Stderr };
```

### Configuration Types (Sources.hpp)

```cpp
struct commands::Properties {
    QString extension;          // File extension
    QString exec;               // Command to execute
    QVariantMap parameters;     // Optional params
    bool restart;               // Auto-restart on exit
};

struct sources::Properties {
    QString type;               // "local", "ssh", "ssh2"
    QString host;               // For ssh/ssh2
    Commands commands;          // Map<name, Properties>
    bool reconnect;             // Auto-reconnect on disconnect
    QVariantMap parameters;     // ssh connection params
};

using Sources = QMap<QString, sources::Properties>;
```

## Key Classes & Interfaces

### IProvider (providers/IProvider.hpp)
- `start()`, `stop()` - lifecycle
- `type()` - returns provider type string
- `commands()` - returns configured commands
- `state()` - current DaggyProviderStates
- Emits signals on state/stream changes

### IAggregator (aggregators/IAggregator.hpp)
- `isReady()` - check readiness before use
- Slots for all provider events
- Processes incoming streams based on type

### Core API (Core.hpp)
- Main entry point for library users
- Manages providers and aggregators lifecycle
- Emits DaggyStates changes

## Common Implementation Patterns

### Add New Provider Type

1. Create `CSomething.cpp/hpp` in `src/DaggyCore/providers/`
2. Inherit from `IProvider`
3. Implement `start()`, `stop()`, `type()`
4. Emit signals: `setState()`, `commandStream()`, `commandStateChanged()`
5. Create factory class `CSomethingFabric` inheriting `IFabric`
6. Register in provider factory logic

### Add New Aggregator Type

1. Create `CSomething.cpp/hpp` in `src/DaggyCore/aggregators/`
2. Inherit from `IAggregator`
3. Implement all virtual slots
4. Connect to provider signals
5. Process streams in `onCommandStream()`

### Configuration Example

```yaml
sources:
  local:
    type: local
    commands:
      - name: cmd1
        command: ping ya.ru
        extension: log
  remote:
    type: ssh
    host: 192.168.1.100
    connection:
      login: user
      key: ~/.ssh/id_rsa
      port: 22
    commands:
      - name: cmd2
        command: df -h
        extension: log
```

## Build & Test

### Build
```bash
# With Conan
conan install . --build=missing
cmake --preset conan-release
cmake --build --preset conan-release

# Without Conan (development)
mkdir build && cd build
cmake ..
cmake --build . -j4
```

### Run
```bash
# Console app
./build/bin/daggy config.yaml -o output_dir

# From stdin
cat config.yaml | ./build/bin/daggy -f yaml -i

# With output folder
./build/bin/daggy config.yaml -o ./results
```

### Test
```bash
ctest --preset conan-release
# or
cmake --build build --target test
```

## Error Handling

- All functions return `std::error_code`
- Errors also emitted as signals from providers
- Aggregators receive errors via `onCommandError()` and `onDataProviderError()`
- Check `Errors.h` for error categories and codes

## File I/O

### Input
- Configuration: YAML/JSON parsed via `sources::convertors::yaml()` or `json()`
- Parsed into `Sources` map (type QMap<QString, sources::Properties>)

### Output (File Aggregator)
- Creates output directory: `{date}-{config_basename}/`
- Files: `{hostname}_{command_name}.{extension}`
- Streamed writes (minimal memory)

## Qt Integration

- Uses Qt6 `QObject` for signal/slot mechanism
- `QMap`, `QString`, `QByteArray` for data types
- No GUI - purely C++ core library
- Console app uses minimal Qt (mainly for event loop)

## Key Signals/Slots

**Provider → Aggregator:**
```
commandStream(QString id, Stream data)
commandStateChanged(QString id, DaggyCommandStates state, int exit_code)
commandError(QString id, std::error_code error_code)
stateChanged(DaggyProviderStates state)
error(std::error_code error_code)
```

**Core → Aggregator:**
```
onDaggyStateChanged(DaggyStates state)
```

## Environment & Versioning

- Version in: `src/cmake/version.cmake`
- C++ standard: C++23
- Qt version: 6.0+
- CMake: 3.20+

## Dependencies

Core (required):
- Qt (Core module minimum)
- libssh2 (for SSH providers)
- libyaml (for YAML parsing)
- nlohmann/json (for JSON parsing)

Optional:
- OpenSSL (for SSH/TLS)
- zlib (compression)

## Common Issues & Solutions

| Issue | Cause | Solution |
|-------|-------|----------|
| SSH connection timeout | Firewall/network | Check host, port, credentials |
| Command not found | Remote path differs | Use full path in command |
| Permission denied | SSH key/sudoers | Check SSH key permissions (600) |
| Output file empty | Command failed silently | Check command state signal |
| Reconnect loop | Server unreachable | Set reconnect=false |

## Performance Tips

- Use `local` type for same-machine commands (no overhead)
- SSH connection pooling handled per-provider
- Streams are chunked (check `CFile` buffer size)
- Qt event loop scales with command count
- Use `extension: log` for text, appropriate types for binary

## Documentation Files

- `docs/README.md` - Overview and getting started
- `docs/how-it-works.md` - Workflow and CLI usage
- `docs/daggy-core/c++23-interface.md` - Library API
- `docs/data-aggregation-config.md` - Configuration format
- `docs/daggy_*.plantuml` - Architecture diagrams (PlantUML)
