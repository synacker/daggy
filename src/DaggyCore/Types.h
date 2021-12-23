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
#pragma once

#include <time.h>
#include <stdint.h>

#include "Errors.h"

#ifdef __cplusplus
extern "C" {
#endif

enum DaggyProviderStates {
    DaggyProviderNotStarted,
    DaggyProviderStarting,
    DaggyProviderFailedToStart,
    DaggyProviderStarted,
    DaggyProviderFinishing,
    DaggyProviderFinished
};

enum DaggyCommandStates {
    DaggyCommandNotStarted,
    DaggyCommandStarting,
    DaggyCommandStarted,
    DaggyCommandFailedToStart,
    DaggyCommandFinishing,
    DaggyCommandFinished
};

enum DaggyStreamTypes {
    DaggyStreamStandard,
    DaggyStreamError
};

enum DaggyStates {
    DaggyNotStarted,
    DaggyStarted,
    DaggyFinishing,
    DaggyFinished
};

enum DaggySourcesTextTypes {
    Json,
    Yaml
};

struct {
    time_t start_time;
    const char* extension;
    DaggyStreamTypes type;

    uint64_t seq_num;
    time_t time;

    const uint8_t* data;
    int64_t size;

} typedef DaggyStream;

struct {
    const char* full;
    const std::uint16_t major;
    const std::uint16_t minor;
    const std::uint16_t patch;
    const std::uint16_t build;
    const char* postfix;
    const char* vendor;
    const char* commit;
} typedef DaggyVersion;

typedef void* DaggyCore;

typedef void (*libdaggy_on_daggy_state_changed)(DaggyCore, DaggyStates);

typedef void (*libdaggy_on_provider_state_changed)(DaggyCore, const char*, DaggyProviderStates);
typedef void (*libdaggy_on_provider_error)(DaggyCore, const char*, DaggyError);

typedef void (*libdaggy_on_command_state_changed)(DaggyCore, const char*, const char*, DaggyCommandStates, int);
typedef void (*libdaggy_on_command_stream)(DaggyCore, const char*, const char*, DaggyStream);
typedef void (*libdaggy_on_command_error)(DaggyCore, const char*, const char*, DaggyError);

#ifdef __cplusplus
}
#endif
