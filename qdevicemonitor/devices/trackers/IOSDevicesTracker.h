/*
    This file is part of QDeviceMonitor.

    QDeviceMonitor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QDeviceMonitor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QDeviceMonitor. If not, see <http://www.gnu.org/licenses/>.
*/

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
