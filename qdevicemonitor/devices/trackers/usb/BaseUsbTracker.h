#ifndef BASEUSBTRACKER_H
#define BASEUSBTRACKER_H

#include <QObject>
#include <QSharedPointer>

class BaseUsbTracker : public QObject
{
    Q_OBJECT

public:
    virtual ~BaseUsbTracker()
    {
    }

    static QSharedPointer<BaseUsbTracker> create();

signals:
    void usbConnectionChanged();
};

#endif
