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
#include "Precompiled.hpp"
#include "Errors.hpp"


const std::error_code daggy::errors::success = std::error_code(daggy::errors::make_error_code(DaggyErrorSuccess));

namespace  {
class DaggyErrorCategory : public std::error_category
{
public:
    const char* name() const noexcept override
    {
        return "DaggyErrors";
    };
    std::string message(int ev) const override {
        switch (static_cast<DaggyErrors>(ev)) {
        case DaggyErrorSuccess: return "Success";
        case DaggyErrorAggregatorCannotConnect: return "Cannot connect aggregator";
        case DaggyErrorProviderCannotPrepare: return "Cannot prepare providers";
        case DaggyErrorSourceIncorrectProviderType: return "Incorrect provider type";
        case DaggyErrorSourceProviderTypeIsNotSupported: return "Data provider type is not supported";
        case DaggyErrorSourceNullCommand: return "Null command";
        case DaggyErrorCommandRead: return "Command read error";
        case DaggyErrorProviderFailedToStart: return "Provider failed to start";
        case DaggyErrorProviderFailedToStop: return "Provider failed to stop";
        case DaggyErrorProviderAlreadyStarted: return "Provider already started";
        case DaggyErrorProviderAlreadyFinished: return "Provider already finished";
        case DaggyErrorStreamCorrupted: return "Stream corrupted";
        case DaggyErrorAlreadyStarted: return "Daggy already started";
        case DaggyErrorAlreadyFinished: return "Daggy already finished";
        case DaggyErrorInternal: return "Daggy internal error";
        default: return "Unknown error";
        }
    };

};

thread_local static DaggyErrorCategory daggy_error_category;
}

const std::error_category& daggy::errors::category() noexcept
{
    return daggy_error_category;
}

std::error_code daggy::errors::make_error_code(DaggyErrors error) noexcept
{
    return std::error_code(static_cast<int>(error), category());
}
