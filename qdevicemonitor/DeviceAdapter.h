#ifndef DEVICEADAPTER_H
#define DEVICEADAPTER_H

#include "DataTypes.h"

#include <QObject>
#include <QPointer>
#include <QTabWidget>
#include <QTimer>

class DeviceAdapter : public QObject
{
    Q_OBJECT

    static const int UPDATE_FREQUENCY = 100;

private:
    DataTypes::DevicesMap m_devicesMap;
    QTimer m_updateTimer;

public:
    explicit DeviceAdapter(QPointer<QTabWidget> parent = 0);

signals:

public slots:
    void update();

private:
    void updateDevicesMap();
};

#endif // DEVICEADAPTER_H
