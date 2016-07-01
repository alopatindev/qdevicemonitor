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

#include "LibusbImpl.h"

#include <libusb.h>
#include <stdio.h>
#include <sys/time.h>

static void (*sHotplugCallback)() = NULL;

bool init()
{
    const bool success = libusb_init(NULL) == 0;
#ifdef QT_DEBUG
    //libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_DEBUG);
#endif
    return success;
}

void release()
{
    sHotplugCallback = NULL;
    libusb_exit(NULL);
}

bool supportsHotplug()
{
    return libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG) != 0;
}

bool update()
{
    struct timeval zeroTimeout;
    zeroTimeout.tv_sec = 0;
    zeroTimeout.tv_usec = 5000;

    const bool success = libusb_handle_events_timeout_completed(NULL, &zeroTimeout, NULL) == 0;
    return success;
}

static int hotplugCallback(libusb_context* context, libusb_device* device, libusb_hotplug_event event, void* userData)
{
    (void) context;
    (void) device;
    (void) userData;

#ifdef QT_DEBUG
    const char* const eventString = (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) ? "disconnected" : "connected";
    printf("usb event: %s\n", eventString);
#else
    (void) event;
#endif

    if (sHotplugCallback != NULL)
    {
        sHotplugCallback();
    }

    return 0;
}

bool registerHotplugCallback(void (*callback)())
{
    sHotplugCallback = callback;

    const libusb_hotplug_event events = LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT;
    const libusb_hotplug_flag flags = LIBUSB_HOTPLUG_ENUMERATE;

    const bool hotplugRegistered = libusb_hotplug_register_callback(
        NULL,
        events,
        flags,
        LIBUSB_HOTPLUG_MATCH_ANY,
        LIBUSB_HOTPLUG_MATCH_ANY,
        LIBUSB_HOTPLUG_MATCH_ANY,
        &hotplugCallback,
        NULL,
        NULL
    ) == LIBUSB_SUCCESS;

    return hotplugRegistered;
}
