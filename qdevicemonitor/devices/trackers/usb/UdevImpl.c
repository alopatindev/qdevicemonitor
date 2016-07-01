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

#include "UdevImpl.h"

#include <libudev.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>

static const bool s_verboseDebug =
    #ifdef QT_DEBUG
        true
    #else
        false
    #endif
    ;

static struct udev* udev_instance = NULL;
static struct udev_monitor* mon = NULL;
static int fd = 0;
static bool dirtyNewDevice = false;

void init()
{
    udev_instance = udev_new();

    if (udev_instance != NULL)
    {
        mon = udev_monitor_new_from_netlink(udev_instance, "udev");
        udev_monitor_filter_add_match_subsystem_devtype(mon, "usb", "usb_device");
        udev_monitor_enable_receiving(mon);
        fd = udev_monitor_get_fd(mon);
    }
}

void release()
{
    if (mon != NULL)
    {
        udev_monitor_unref(mon);
        mon = NULL;
    }

    if (udev_instance != NULL)
    {
        udev_unref(udev_instance);
        udev_instance = NULL;
    }
}

bool isAvailable()
{
    return udev_instance != NULL && mon != NULL;
}

void update()
{
    if (!isAvailable())
    {
        return;
    }

    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    const int ret = select(fd + 1, &fds, NULL, NULL, &tv);
    const bool receivedData = ret > 0 && FD_ISSET(fd, &fds);
    if (receivedData)
    {
        struct udev_device* dev = udev_monitor_receive_device(mon);
        if (dev != NULL)
        {
            printf("udev: Got Device\n");
            dirtyNewDevice = true;

            if (s_verboseDebug)
            {
                printf(" Node %s\n", udev_device_get_devnode(dev));
                printf(" Subsystem %s\n", udev_device_get_subsystem(dev));
                printf(" Devtype %s\n", udev_device_get_devtype(dev));
                printf(" Action %s\n", udev_device_get_action(dev));
            }

            udev_device_unref(dev);
        }
        else
        {
            printf("udev: No Device from receive_device(). An error occured.\n");
        }
    }
}

bool hasNewDevice()
{
    if (dirtyNewDevice)
    {
        dirtyNewDevice = false;
        return true;
    }
    else
    {
        return false;
    }
}
