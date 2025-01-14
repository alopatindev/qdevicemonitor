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

#include "IOSDevicesTracker.h"

#include <QDebug>

IOSDevicesTracker::IOSDevicesTracker()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    m_codecStream.setCodec("UTF-8");
#else
    m_codecStream.setEncoding(QStringConverter::Utf8);
#endif
    m_codecStream.setString(&m_buffer, QIODevice::ReadWrite | QIODevice::Text);
}

IOSDevicesTracker::~IOSDevicesTracker()
{
    stopListProcess();
}

void IOSDevicesTracker::update()
{
    if (m_listProcess.state() == QProcess::NotRunning)
    {
        updateConnectedDevices();
        stopListProcess();
        startListProcess();
    }
}

void IOSDevicesTracker::startListProcess()
{
    if (m_listProcess.state() == QProcess::NotRunning)
    {
        QStringList args;
        args.append("-l");
        m_listProcess.setReadChannel(QProcess::StandardOutput);
        m_listProcess.start("idevice_id", args);
    }
}

void IOSDevicesTracker::stopListProcess()
{
    if (m_listProcess.state() != QProcess::NotRunning)
    {
        m_listProcess.kill();
    }
}

bool IOSDevicesTracker::checkErrors()
{
    bool result = false;

    QString errorText;
    if (m_listProcess.exitCode() != 0 ||
        m_listProcess.exitStatus() == QProcess::ExitStatus::CrashExit)
    {
        result = true;

        m_codecStream << m_listProcess.readAllStandardError();
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
        // FIXME: remove this hack
        errorText = m_codecStream.readLine();
#else
        (void) m_codecStream.readLineInto(&errorText);
#endif
        const bool listProcessFailed =
            m_listProcess.exitCode() != 0xFF ||
            errorText != "ERROR: Unable to retrieve device list!";

        if (listProcessFailed)
        {
            qDebug() << "IOSDevicesTracker::m_listProcess exitCode" << m_listProcess.exitCode()
                     << "; exitStatus" << m_listProcess.exitStatus()
                     << "; stderr" << errorText;
        }
        else
        {
#if defined(Q_OS_LINUX)
            // TODO: if ps uax | grep usbmuxd
            result = false;
#endif
        }
    }

    return result;
}

void IOSDevicesTracker::updateConnectedDevices()
{
    bool listError = checkErrors();
    if (listError)
    {
        return;
    }

    updateNextConnectedDevices();
    updateDisconnectedDevices();

    m_connectedDevices = m_nextConnectedDevices;
}

void IOSDevicesTracker::updateNextConnectedDevices()
{
    m_nextConnectedDevices.clear();

    if (m_listProcess.canReadLine())
    {
        m_codecStream << m_listProcess.readAll();
    }

    QString deviceId;
    while (!m_codecStream.atEnd())
    {
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
        // FIXME: remove this hack
        deviceId = m_codecStream.readLine();
        const bool lineIsRead = !deviceId.isEmpty();
#else
        const bool lineIsRead = m_codecStream.readLineInto(&deviceId);
#endif
        if (!lineIsRead)
        {
            break;
        }

        if (!m_connectedDevices.contains(deviceId))
        {
            emit deviceConnected(DataTypes::DeviceType::IOS, deviceId);
        }

        m_nextConnectedDevices.insert(deviceId);
    }
}

void IOSDevicesTracker::updateDisconnectedDevices()
{
    const auto disconnectedDevices = m_connectedDevices - m_nextConnectedDevices;
    for (auto it = disconnectedDevices.constBegin(); it != disconnectedDevices.constEnd(); ++it)
    {
        emit deviceDisconnected(DataTypes::DeviceType::IOS, *it);
    }
}
