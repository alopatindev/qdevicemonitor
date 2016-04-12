#ifndef TIMERUSBTRACKER_H
#define TIMERUSBTRACKER_H

#include "BaseUsbTracker.h"
#include <QTimer>

class TimerUsbTracker : public BaseUsbTracker
{
    QTimer m_updateTimer;

    static const int UPDATE_FREQUENCY = 10 * 1000;

public:
    TimerUsbTracker();
    ~TimerUsbTracker() override;
};

#endif
