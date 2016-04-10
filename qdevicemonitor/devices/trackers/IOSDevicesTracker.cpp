#include "IOSDevicesTracker.h"

#include <QDebug>

IOSDevicesTracker::IOSDevicesTracker()
{
    m_codecStream.setCodec("UTF-8");
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
    QStringList args;
    args.append("-l");
    m_listProcess.setReadChannel(QProcess::StandardOutput);
    m_listProcess.start("idevice_id", args);
}

void IOSDevicesTracker::stopListProcess()
{
    if (m_listProcess.state() != QProcess::NotRunning)
    {
        m_listProcess.terminate();
        m_listProcess.kill();
        m_listProcess.close();
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
        (void) m_codecStream.readLineInto(&errorText);
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
        const bool lineIsRead = m_codecStream.readLineInto(&deviceId);
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
