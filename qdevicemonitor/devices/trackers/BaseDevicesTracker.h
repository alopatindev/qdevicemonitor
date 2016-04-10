#ifndef BASEDEVICESTRACKER_H
#define BASEDEVICESTRACKER_H

#include <QString>
#include <QTimer>
#include "DataTypes.h"

class BaseDevicesTracker : public QObject
{
    Q_OBJECT

    static const int UPDATE_FREQUENCY = 20;
    QTimer m_updateTimer;

private slots:
    virtual void update() = 0;

public:
    BaseDevicesTracker()
    {
        connect(&m_updateTimer, &QTimer::timeout, this, &BaseDevicesTracker::update);
        m_updateTimer.start(UPDATE_FREQUENCY);
    }

    virtual ~BaseDevicesTracker()
    {
        disconnect(&m_updateTimer, &QTimer::timeout, this, &BaseDevicesTracker::update);
    }

signals:
    void deviceConnected(const DataTypes::DeviceType type, const QString& id);
    void deviceDisconnected(const DataTypes::DeviceType type, const QString& id);
};

#endif // BASEDEVICESTRACKER_H
