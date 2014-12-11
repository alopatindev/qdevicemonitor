/*
    This file is part of QDeviceMonitor.

    QDeviceMonitor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QDeviceMonitor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QDeviceMonitor. If not, see <http://www.gnu.org/licenses/>.
*/

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

    enum VerbosityEnum
    {
        Fatal,
        Error,
        Warn,
        Info,
        Debug,
        Verbose
    };
}

#endif // DATATYPES_H
