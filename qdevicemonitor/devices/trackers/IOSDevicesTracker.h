#ifndef IOSDEVICESTRACKER_H
#define IOSDEVICESTRACKER_H

#include "BaseDevicesTracker.h"
#include <QProcess>
#include <QSet>
#include <QString>
#include <QTextStream>

class IOSDevicesTracker : public BaseDevicesTracker
{
    QProcess m_listProcess;

    QSet<QString> m_connectedDevices;
    QSet<QString> m_nextConnectedDevices;

    QString m_buffer;
    QTextStream m_codecStream;

public:
    IOSDevicesTracker();
    ~IOSDevicesTracker() override;

private slots:
    void update() override;

private:
    void startListProcess();
    void stopListProcess();
    bool checkErrors();

    void updateConnectedDevices();
    void updateNextConnectedDevices();
    void updateDisconnectedDevices();
};

#endif // IOSDEVICESTRACKER_H
