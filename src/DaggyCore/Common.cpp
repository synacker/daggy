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
#include "Common.h"

using namespace daggycore;

const std::error_code daggycore::success = std::error_code{};

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
        case CannotPrepareAggregator: return "Cannot prepare aggregator";
        case IncorrectParameterName: return "Incorrect field name";
        case WrongSourceParameter: return "Wrong parameter type";
        case IncorrectProviderType: return "Incorrect provider type";
        case ProviderTypeAlreadyExists: return "The parent of object is not nullptr";
        case NotAllowed: return "Operation is not allowed in current state";
        case DataProviderTypeIsNotSupported: return "Data provider type is not supported";
        case NullCommand: return "Null command";
        case CommandReadError: return "Command read error";
        default: return "Unknown error";
        }
    };

} daggy_error_category;

}

std::error_code daggycore::make_error_code(DaggyErrors daggy_error)
{
    return std::error_code(static_cast<int>(daggy_error), daggy_error_category);
}
