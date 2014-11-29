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

    enum Verbosity
    {
        Verbose,
        Debug,
        Info,
        Warn,
        Error,
        Fatal,

        VerbosityEnd
    };
}

#endif // DATATYPES_H
