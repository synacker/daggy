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
#include "Precompiled.h"
#include "IDataSourceConvertor.h"

#include "Common.h"

using namespace daggycore;
using namespace daggyconv;

const QMap<const char*, QVariant::Type> IDataSourceConvertor::required_source_field =
{
    {IDataSourceConvertor::g_hostField, QVariant::String},
    {IDataSourceConvertor::g_typeField, QVariant::String},
    {IDataSourceConvertor::g_commandsField, QVariant::Map}
};

const QMap<const char*, QVariant::Type> IDataSourceConvertor::required_commands_field =
{
    {IDataSourceConvertor::g_execField, QVariant::String},
    {IDataSourceConvertor::g_extensionField, QVariant::String}
};

IDataSourceConvertor::IDataSourceConvertor(QString type_arg, QObject* parent)
    : QObject(parent)
    , type(std::move(type_arg))
{
    
}
