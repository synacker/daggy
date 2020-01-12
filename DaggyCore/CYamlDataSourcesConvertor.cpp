/*
Copyright 2017-2020 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "Precompiled.h"
#include "CYamlDataSourcesConvertor.h"

using namespace daggycore;

namespace YAML {

/*= QString
   =========================================================================================*/
template<>
struct convert<QString>
{
  static Node encode(const QString& rhs) {
    Node node;
    node = rhs.toStdString();
    return node;
  }

  static bool decode(const Node& node, QString& rhs) {
    if (!node.IsScalar()) {
      return false;
    }

    std::string sstr = node.as<std::string>();
    rhs = QString(sstr.c_str());

    return true;
  }
};

template<>
struct convert<QVariant>
{
  static Node encode(const QVariant& rhs) {
    Node node;
    node = rhs;
    return node;
  }

  static bool decode(const Node& node, QVariant& rhs) {
    if (!node.IsScalar()) {
      return false;
    }

    //      std::string sstr = node
    rhs = QVariant(node);

    return true;
  }
};

/*= QByteArray
   ======================================================================================*/
template<>
struct convert<QByteArray>
{
  static Node encode(const QByteArray& rhs) {
    Node node;
    const char* data = rhs.constData();
    YAML::Binary binary(reinterpret_cast<const unsigned char*>(data),
                        size_t(rhs.size()));
    node = binary;
    return node;
  }

  static bool decode(const Node& node, QByteArray& rhs) {
    if (!node.IsScalar()) {
      return false;
    }

    YAML::Binary binary = node.as<YAML::Binary>();
    const char* data = reinterpret_cast<const char*>(binary.data());
    int size = int(binary.size());
    rhs = QByteArray(data, size);

    return true;
  }
};

void operator>>(const Node& node, QString& q)
{
  std::string sstr;
  sstr = node.as<std::string>();
  q = QString(sstr.c_str());
}

void operator<<(Node& node, const QString& q)
{
  std::string sstr = q.toStdString();
  node = sstr;
}

void operator>>(const Node& node, QByteArray& q)
{
  YAML::Binary binary = node.as<YAML::Binary>();
  const char* data = reinterpret_cast<const char*>(binary.data());
  int size = int(binary.size());
  q = QByteArray(data, size);
}

void operator<<(Node& node, const QByteArray& q)
{
  const char* data = q.constData();
  YAML::Binary binary(reinterpret_cast<const unsigned char*>(data),
                      size_t(q.size()));
  node = binary;
}

}


QString CYamlDataSourcesConvertor::convert(const DataSources& data_sources) const
{
    QString result;
    return result;
}

DataSources CYamlDataSourcesConvertor::convert(const QString& data_sources) const
{
    DataSources result;
    YAML::Node node = YAML::Load(data_sources.toStdString());

    return result;
}

QString CYamlDataSourcesConvertor::type() const
{
    return "yaml";
}
