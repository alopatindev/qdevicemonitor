#include "BaseUsbTracker.h"

#include "TimerUsbTracker.h"

#if defined(Q_OS_LINUX)
    #include "UdevUsbTracker.h"
#endif

QSharedPointer<BaseUsbTracker> BaseUsbTracker::create()
{
#if defined(Q_OS_LINUX)
    auto tracker = QSharedPointer<UdevUsbTracker>::create();
    if (tracker->isAvailable())
    {
        return tracker;
    }
    else
    {
        return QSharedPointer<TimerUsbTracker>::create();
    }
#else
    return QSharedPointer<TimerUsbTracker>::create();
#endif
}
