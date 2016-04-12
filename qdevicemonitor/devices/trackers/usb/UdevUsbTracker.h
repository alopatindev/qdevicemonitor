#ifndef UDEVUSBTRACKER_H
#define UDEVUSBTRACKER_H

#include "BaseUsbTracker.h"
#include <QTimer>

namespace udev
{
    extern "C"
    {
        #include <libudev.h>
    }
}

class UdevUsbTracker : public BaseUsbTracker
{
    QTimer m_updateTimer;

    static const int UPDATE_FREQUENCY = 1000;

    udev::udev* m_udev;
    udev::udev_monitor* m_mon;
    int m_fd;

public:
    UdevUsbTracker();
    ~UdevUsbTracker() override;
    bool isAvailable() const;

private:
    void update();
};

#endif
