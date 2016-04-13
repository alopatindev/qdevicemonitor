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

#include "BaseUsbTracker.h"

#include "TimerUsbTracker.h"

#if defined(Q_OS_LINUX)
    #include "UdevUsbTracker.h"
#endif

QSharedPointer<BaseUsbTracker> BaseUsbTracker::create()
{
#if defined(Q_OS_LINUX)
    auto tracker = QSharedPointer<UdevUsbTracker>::create();
    if (tracker->isAvailable())
    {
        return tracker;
    }
    else
    {
        return QSharedPointer<TimerUsbTracker>::create();
    }
#else
    return QSharedPointer<TimerUsbTracker>::create();
#endif
}
