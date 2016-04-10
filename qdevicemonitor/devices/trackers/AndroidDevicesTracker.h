#ifndef ANDROIDDEVICESTRACKER_H
#define ANDROIDDEVICESTRACKER_H

#include "BaseDevicesTracker.h"
#include <QProcess>
#include <QSet>
#include <QString>
#include <QTextStream>

class AndroidDevicesTracker : public BaseDevicesTracker
{
    QProcess m_listProcess;

    QSet<QString> m_connectedDevices;
    QSet<QString> m_nextConnectedDevices;

    QString m_buffer;
    QTextStream m_codecStream;

public:
    AndroidDevicesTracker();
    ~AndroidDevicesTracker() override;

private slots:
    void update() override;

private:
    void startListProcess();
    void stopListProcess();

    void updateConnectedDevices();
    void updateNextConnectedDevices();
    void updateDisconnectedDevices();
};

#endif // ANDROIDDEVICESTRACKER_H
