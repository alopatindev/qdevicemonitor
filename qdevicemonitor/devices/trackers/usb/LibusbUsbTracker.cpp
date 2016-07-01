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
    maybeRegisterHotplugCallback();

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
    const bool success = libusb::update();
    if (!success)
    {
        qDebug() << "libusb: handing events FAILED";
    }
}

void LibusbUsbTracker::initLibusb()
{
    Q_ASSERT_X(m_initialized == false, "LibusbUsbTracker::initLibusb", "already initialized");
    m_initialized = libusb::init();
}

void LibusbUsbTracker::maybeReleaseLibusb()
{
    if (m_initialized)
    {
        qDebug() << "libusb_exit";
        libusb::release();
        m_initialized = false;
        m_hotplugRegistered = false;
    }
}

void LibusbUsbTracker::maybeRegisterHotplugCallback()
{
    Q_ASSERT_X(m_hotplugRegistered == false, "LibusbUsbTracker::maybeRegisterHotplugCallback", "already registered");

    const bool supportsHotplug = m_initialized && libusb::supportsHotplug();
    if (supportsHotplug)
    {
        m_hotplugRegistered = libusb::registerHotplugCallback(&hotplugCallbackStatic);
    }
}

void LibusbUsbTracker::hotplugCallback()
{
    qDebug() << "LibusbUsbTracker::handleConnectionChanged";
    emit usbConnectionChanged();
}

void LibusbUsbTracker::hotplugCallbackStatic()
{
    s_this->hotplugCallback();
}
