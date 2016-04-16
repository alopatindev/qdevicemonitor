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

#ifndef COLORTHEME_H
#define COLORTHEME_H

#include <QtGlobal>
#include <QSharedPointer>

class ColorTheme
{
public:
    enum ColorType
    {
        VerbosityAssert,
        VerbosityError,
        VerbosityWarn,
        VerbosityInfo,
        VerbosityDebug,
        VerbosityVerbose,

        DateTime,
        Pid,
        Tid,
        Tag
    };

    static QSharedPointer<ColorTheme> create(const bool darkTheme);

    virtual Qt::GlobalColor getColor(const ColorType type) const = 0;

    virtual ~ColorTheme()
    {
    }
};

#endif
