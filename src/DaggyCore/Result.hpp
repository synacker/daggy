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
#pragma once
#include <system_error>
#include <utility>
#include <optional>
#include <QString>

#include <DaggyCore/daggycore_export.h>
#include "Errors.hpp"

namespace daggy {

template<typename Data>
class Result
{
public:
    Result() = delete;
    Result(Data data)
        : data_(std::move(data))
        , error(errors::success)
    {}

    Result(std::error_code error,
           QString message = QString())
        : data_{}
        , error(std::move(error))
        , message(std::move(message))
    {}

    Result(const Result&) = delete;
    Result(Result&&) = default;

    operator bool() const
    {
        return !error;
    }

    Data&& operator*() {
        return std::move(data_.operator*());
    }

    const Data& operator *() const {
        return data_.operator*();
    }

    const Data* operator->() const {
        return data_.operator->();
    }

    Data* operator->() {
        return data_.operator->();
    }

    const Data* data() const {
        return &data_.value();
    }

    Data* data() {
        return &data_.value();
    }

    const std::error_code error;
    const QString message;

private:
    std::optional<Data> data_;
};


}
