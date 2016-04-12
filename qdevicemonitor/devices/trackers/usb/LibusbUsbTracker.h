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

    static const int UPDATE_FREQUENCY = 1000;

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
