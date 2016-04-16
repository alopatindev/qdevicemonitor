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

#ifndef LIGHTCOLORTHEME_H
#define LIGHTCOLORTHEME_H

#include "ColorTheme.h"
#include <QVector>

class LightColorTheme : public ColorTheme
{
    QVector<Qt::GlobalColor> m_colors;

public:
    LightColorTheme()
        : m_colors({
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
        })
    {
    }

    Qt::GlobalColor getColor(const ColorType type) const override
    {
        return m_colors[type];
    }
};

#endif
