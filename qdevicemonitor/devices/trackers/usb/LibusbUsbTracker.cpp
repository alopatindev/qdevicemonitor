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

#include "LibusbUsbTracker.h"
#include <QDebug>
#include <QPointer>

static QPointer<LibusbUsbTracker> s_this;

LibusbUsbTracker::LibusbUsbTracker()
    : m_initialized(false)
    , m_hotplugRegistered(false)
{
    Q_ASSERT_X(s_this.isNull(), "LibusbUsbTracker", "only one instance is allowed");
    s_this = this;

    connect(&m_updateTimer, &QTimer::timeout, this, &LibusbUsbTracker::update);

    initLibusb();
    registerHotplugCallback();

    if (m_hotplugRegistered)
    {
        qDebug() << "LibusbUsbTracker: registered";
        m_updateTimer.start(UPDATE_INTERVAL);
    }
    else
    {
        qDebug() << "LibusbUsbTracker: register failed; using update timer";
        maybeReleaseLibusb();
    }
}

LibusbUsbTracker::~LibusbUsbTracker()
{
    disconnect(&m_updateTimer, &QTimer::timeout, this, &LibusbUsbTracker::update);
    m_updateTimer.stop();
    maybeReleaseLibusb();
}

void LibusbUsbTracker::update()
{
    timeval zeroTimeout;
    zeroTimeout.tv_sec = 0;
    zeroTimeout.tv_usec = 5000;
    const bool success =
        libusb::libusb_handle_events_timeout_completed(nullptr, &zeroTimeout, nullptr) == 0;
    if (!success)
    {
        qDebug() << "libusb: handing events FAILED";
    }
}

void LibusbUsbTracker::initLibusb()
{
    Q_ASSERT_X(m_initialized == false, "LibusbUsbTracker::initLibusb", "already initialized");
    m_initialized = libusb::libusb_init(nullptr) == 0;
#ifdef QT_DEBUG
    //libusb::libusb_set_debug(NULL, libusb::LIBUSB_LOG_LEVEL_DEBUG);
#endif
}

void LibusbUsbTracker::maybeReleaseLibusb()
{
    if (m_initialized)
    {
        qDebug() << "libusb_exit";
        libusb::libusb_exit(nullptr);
        m_initialized = false;
        m_hotplugRegistered = false;
    }
}

void LibusbUsbTracker::registerHotplugCallback()
{
    Q_ASSERT_X(m_hotplugRegistered == false, "LibusbUsbTracker::registerHotplugCallback", "already registered");

    const bool supportsHotplug =
        m_initialized &&
        libusb::libusb_has_capability(libusb::LIBUSB_CAP_HAS_HOTPLUG) != 0;

    if (supportsHotplug)
    {
        const auto events = static_cast<libusb::libusb_hotplug_event>(
            libusb::LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | libusb::LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT
        );
        const auto flags = libusb::LIBUSB_HOTPLUG_ENUMERATE;
        m_hotplugRegistered = libusb::libusb_hotplug_register_callback(
            nullptr,
            events,
            flags,
            LIBUSB_HOTPLUG_MATCH_ANY,
            LIBUSB_HOTPLUG_MATCH_ANY,
            LIBUSB_HOTPLUG_MATCH_ANY,
            &LibusbUsbTracker::hotplugCallback,
            nullptr,
            nullptr
        ) == libusb::LIBUSB_SUCCESS;
    }
}

void LibusbUsbTracker::hotplugCallback()
{
    qDebug() << "LibusbUsbTracker::handleConnectionChanged";
    emit usbConnectionChanged();
}

int LibusbUsbTracker::hotplugCallback(libusb::libusb_context* context, libusb::libusb_device* device, libusb::libusb_hotplug_event event, void* userData)
{
    (void) context;
    (void) device;
    (void) userData;

    qDebug()
        << "usb event:"
        << ((event == libusb::LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) ? "disconnected" : "connected");

    s_this->hotplugCallback();

    return 0;
}
