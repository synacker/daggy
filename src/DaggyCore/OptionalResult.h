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
#include <optional>
#include "Result.h"

namespace daggycore {

template<typename Data> class OptionalResult : public std::optional<Data>
{
public:
    OptionalResult() = default;
    OptionalResult(Result result) noexcept
        : result_(std::move(result)){}
    OptionalResult(std::error_code error_code) noexcept
        : result_(std::move(error_code)){}

    OptionalResult(std::error_code error_code,
                   std::string error_message) noexcept
        : result_(std::move(error_code),
                  std::move(error_message)){}


    OptionalResult(Data data) noexcept
        : std::optional<Data>(std::make_optional(std::move(data))){}

    OptionalResult(std::error_code error_code, Data data)
        : result_(std::move(error_code))
        , std::optional<Data>(std::make_optional(std::move(data))){}

    OptionalResult(OptionalResult&& optional_result) = default;

    operator bool() const noexcept {
        return result_ == Result::success &&
               std::optional<Data>::has_value();
    }

    OptionalResult& operator=(const std::error_code& error_code) {
        result_ = error_code;
        return *this;
    }

    OptionalResult& operator=(std::error_code&& error_code) {
        result_ = error_code;
        return *this;
    }

    OptionalResult& operator=(const OptionalResult& other) = default;

    OptionalResult& operator=(OptionalResult&& other) = default;

    OptionalResult& operator=(const Data& data) {
        std::optional<Data>::operator=(data);
        return *this;
    }

    OptionalResult& operator=(const Result& result) {
        result_ = result;
        return *this;
    }

    const Result& result() const {
        return result_;
    }

private:
    Result result_;
};
}
