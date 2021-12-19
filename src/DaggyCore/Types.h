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

#ifdef __cplusplus
}
#endif
