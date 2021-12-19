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
#include "../Precompiled.hpp"
#include "IFabric.hpp"

#include "../Sources.hpp"
#include "../Errors.hpp"

daggy::providers::IFabric::IFabric()
{

}

daggy::providers::IFabric::~IFabric()
{

}

daggy::Result<daggy::providers::IProvider*> daggy::providers::IFabric::create(const Source& source, QObject* parent)
{
    const auto& properties = source.second;
    if (properties.type != type()) {
        return
        {
            errors::make_error_code(DaggyErrorSourceIncorrectProviderType),
            QString("Source %1 has incorrect provider type %2 - type %3 is required").arg(source.first, properties.type, type())
        };
    }
    return createProvider(source, parent);
}
