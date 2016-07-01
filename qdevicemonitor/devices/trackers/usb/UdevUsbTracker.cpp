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

#include "UdevUsbTracker.h"
#include <QDebug>

namespace udev
{
    extern "C"
    {
        #include "UdevImpl.h"
    }
}

UdevUsbTracker::UdevUsbTracker()
{
    qDebug() << "UdevUsbTracker::UdevUsbTracker";

    udev::init();
    if (isAvailable())
    {
        m_updateTimer.start(UPDATE_INTERVAL);
    }
    else
    {
        qDebug() << "failed to initialize udev";
    }

    connect(&m_updateTimer, &QTimer::timeout, this, &UdevUsbTracker::update);
}

UdevUsbTracker::~UdevUsbTracker()
{
    qDebug() << "UdevUsbTracker::~UdevUsbTracker";

    disconnect(&m_updateTimer, &QTimer::timeout, this, &UdevUsbTracker::update);
    m_updateTimer.stop();

    udev::release();
}

void UdevUsbTracker::update()
{
    udev::update();

    if (udev::hasNewDevice())
    {
        emit usbConnectionChanged();
    }
}

bool UdevUsbTracker::isAvailable() const
{
    return udev::isAvailable();
}
