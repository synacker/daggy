#include "Precompiled.h"
#include "CDataSourcesFabric.h"

using namespace dataagregatorcore;

CDataSourcesFabric::CDataSourcesFabric()
{

}

DataSources CDataSourcesFabric::getDataSources(const CDataSourcesFabric::InputTypes input_type, const QByteArray &input) const
{
    DataSources result;
    switch (input_type) {
    case InputTypes::Json:
        result = getFromJson(input);
        break;
    }
    return result;
}

DataSources CDataSourcesFabric::getFromJson(const QByteArray &json) const
{
    DataSources result;
    return result;
}
