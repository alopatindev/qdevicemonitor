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

extern "C"
{
    #include <sys/select.h>
}

static const bool s_verboseDebug = true;

UdevUsbTracker::UdevUsbTracker()
    : m_udev(udev::udev_new())
    , m_mon(nullptr)
    , m_fd(0)
{
    m_udev = udev::udev_new();

    if (m_udev != nullptr)
    {
        m_mon = udev::udev_monitor_new_from_netlink(m_udev, "udev");
        udev::udev_monitor_filter_add_match_subsystem_devtype(m_mon, "usb", "usb_device");
        udev::udev_monitor_enable_receiving(m_mon);
        m_fd = udev::udev_monitor_get_fd(m_mon);
    }

    if (isAvailable())
    {
        m_updateTimer.start(UPDATE_FREQUENCY);
    }
    else
    {
        qDebug() << "failed to initialize udev";
    }

    connect(&m_updateTimer, &QTimer::timeout, this, &UdevUsbTracker::update);
}

UdevUsbTracker::~UdevUsbTracker()
{
    disconnect(&m_updateTimer, &QTimer::timeout, this, &UdevUsbTracker::update);
    m_updateTimer.stop();

    if (m_mon != nullptr)
    {
        udev::udev_monitor_unref(m_mon);
        m_mon = nullptr;
    }

    if (m_udev != nullptr)
    {
        udev::udev_unref(m_udev);
        m_udev = nullptr;
    }
}

void UdevUsbTracker::update()
{
    if (!isAvailable())
    {
        return;
    }

    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(m_fd, &fds);

    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    const int ret = select(m_fd + 1, &fds, NULL, NULL, &tv);
    const bool receivedData = ret > 0 && FD_ISSET(m_fd, &fds);
    if (receivedData)
    {
        udev::udev_device* dev = udev::udev_monitor_receive_device(m_mon);
        if (dev != nullptr)
        {
            qDebug() << "udev: Got Device";
            emit usbConnectionChanged();

            if (s_verboseDebug)
            {
                qDebug() << " Node" << udev::udev_device_get_devnode(dev);
                qDebug() << " Subsystem" << udev::udev_device_get_subsystem(dev);
                qDebug() << " Devtype" << udev::udev_device_get_devtype(dev);
                qDebug() << " Action" << udev::udev_device_get_action(dev);
            }

            udev::udev_device_unref(dev);
        }
        else
        {
            qDebug() << "udev: No Device from receive_device(). An error occured.";
        }
    }
}

bool UdevUsbTracker::isAvailable() const
{
    return m_udev != nullptr && m_mon != nullptr;
}
