#ifndef DATATYPES_H
#define DATATYPES_H

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

        DeviceTypeEnd
    };

    typedef QMap<QString, QSharedPointer<BaseDevice>> DevicesMap;

    static const char* const Verbosity[] = {
        "Fatal",
        "Error",
        "Warn",
        "Info",
        "Debug",
        "Verbose"
    };
}

#endif // DATATYPES_H
