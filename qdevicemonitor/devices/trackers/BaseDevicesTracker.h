#ifndef BASEDEVICESTRACKER_H
#define BASEDEVICESTRACKER_H

#include <QString>
#include <QTimer>
#include "DataTypes.h"

class BaseDevicesTracker : public QObject
{
    Q_OBJECT

public slots:
    virtual void update() = 0;

public:
    virtual ~BaseDevicesTracker()
    {
    }

signals:
    void deviceConnected(const DataTypes::DeviceType type, const QString& id);
    void deviceDisconnected(const DataTypes::DeviceType type, const QString& id);
};

#endif // BASEDEVICESTRACKER_H
