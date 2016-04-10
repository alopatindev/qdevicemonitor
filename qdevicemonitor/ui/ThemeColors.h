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

#ifndef THEMECOLORS_H
#define THEMECOLORS_H

#include <QtGlobal>

namespace ThemeColors
{
    enum Themes
    {
        NormalTheme,
        DarkTheme
    };

    enum ColorTypes
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

    static const Qt::GlobalColor Colors[][10] = {
        {
            Qt::red,
            Qt::red,
            Qt::darkYellow,
            Qt::darkGreen,
            Qt::blue,
            Qt::black,

            Qt::black,
            Qt::darkBlue,
            Qt::blue,
            Qt::darkGreen
        },
        {
            Qt::red,
            Qt::red,
            Qt::yellow,
            Qt::green,
            Qt::lightGray,
            Qt::white,

            Qt::lightGray,
            Qt::green,
            Qt::blue,
            Qt::yellow
        },
    };
}

#endif // THEMECOLORS_H
