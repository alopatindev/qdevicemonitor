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

#ifndef LIBUSBUSBTRACKER_H
#define LIBUSBUSBTRACKER_H

#include "BaseUsbTracker.h"
#include <QTimer>

namespace libusb
{
    extern "C"
    {
        #include <libusb.h>
    }
}

class LibusbUsbTracker : public BaseUsbTracker
{
    bool m_initialized;
    bool m_hotplugRegistered;
    QTimer m_updateTimer;

    static const int UPDATE_INTERVAL = 1000;

public:
    LibusbUsbTracker();
    ~LibusbUsbTracker() override;
    bool isAvailable() const
    {
        return m_hotplugRegistered;
    }

private slots:
    void update();

private:
    void initLibusb();
    void maybeReleaseLibusb();
    void registerHotplugCallback();

    void hotplugCallback();
    static int hotplugCallback(libusb::libusb_context* context, libusb::libusb_device* device, libusb::libusb_hotplug_event event, void* userData);
};

#endif
