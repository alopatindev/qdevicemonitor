#include "TimerUsbTracker.h"

TimerUsbTracker::TimerUsbTracker()
{
    connect(&m_updateTimer, &QTimer::timeout, this, &BaseUsbTracker::usbConnectionChanged);
    m_updateTimer.start(UPDATE_FREQUENCY);
}

TimerUsbTracker::~TimerUsbTracker()
{
    disconnect(&m_updateTimer, &QTimer::timeout, this, &BaseUsbTracker::usbConnectionChanged);
    m_updateTimer.stop();
}
