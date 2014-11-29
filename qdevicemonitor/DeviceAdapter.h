#ifndef DEVICEADAPTER_H
#define DEVICEADAPTER_H

#include "DataTypes.h"

#include <QObject>
#include <QTimer>

class DeviceAdapter : public QObject
{
    Q_OBJECT

private:
    DataTypes::DevicesMap m_devicesMap;
    QTimer m_updateTimer;

public:
    explicit DeviceAdapter(QObject* parent = 0);

signals:

public slots:
    void update();

private:
    void updateDevicesMap();
};

#endif // DEVICEADAPTER_H
