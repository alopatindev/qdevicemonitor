#ifndef DATATYPES_H
#define DATATYPES_H

//#include "BaseDevice.h"
class BaseDevice;

#include <QMap>
#include <QSharedPointer>
#include <QString>

namespace DataTypes
{
    enum DeviceType
    {
        TextFile,
        Android,
        IOS,

        End
    };

    typedef QMap<QString, QSharedPointer<BaseDevice>> DevicesMap;
};

#endif // DATATYPES_H
