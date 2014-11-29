#include "AndroidDevice.h"
#include <QTextStream>
#include <QDebug>

using namespace DataTypes;

AndroidDevice::AndroidDevice(QPointer<QTabWidget> parent, const QString& id, DeviceType type,
                             const QString& humanReadableName, const QString& humanReadableDescription)
    : BaseDevice(parent, id, type, humanReadableName, humanReadableDescription)
{
    updateDeviceModel();
    startLogger();
}

AndroidDevice::~AndroidDevice()
{
    if (m_deviceInfoProcess.state() != QProcess::NotRunning)
    {
        m_deviceInfoProcess.close();
    }

    if (m_deviceLogProcess.state() != QProcess::NotRunning)
    {
        m_deviceLogProcess.close();
    }
}

void AndroidDevice::updateDeviceModel()
{
    QStringList args;
    args.append("-s");
    args.append(m_id);
    args.append("shell");
    args.append("getprop");
    args.append("ro.product.model");
    m_deviceInfoProcess.start("adb", args);
    m_deviceInfoProcess.setReadChannel(QProcess::StandardOutput);
}

void AndroidDevice::startLogger()
{
    QStringList args;
    args.append("-s");
    args.append(m_id);
    args.append("logcat");
    args.append("*:v");
    m_deviceLogProcess.start("adb", args);
    m_deviceLogProcess.setReadChannel(QProcess::StandardOutput);
}

void AndroidDevice::update()
{
    if (m_deviceInfoProcess.state() == QProcess::NotRunning && m_deviceInfoProcess.canReadLine())
    {
        QString streamString;
        QTextStream stream;
        QString model = m_deviceInfoProcess.readLine().trimmed();
        m_deviceInfoProcess.close();
        qDebug() << "model of" << m_id << "is" << model;
        if (!model.isEmpty())
        {
            m_humanReadableName = model;
            updateTabWidget();
        }
    }

    if (m_deviceLogProcess.state() == QProcess::Running && m_deviceLogProcess.canReadLine())
    {
        QString streamString;
        QTextStream stream;
        stream.setString(&streamString, QIODevice::ReadOnly | QIODevice::Text);
        stream << m_deviceLogProcess.readAll();
        while (!stream.atEnd())
        {
            QString line = stream.readLine();
            m_deviceWidget->getTextEdit().append(line);
        }
    }
}

void AndroidDevice::addNewDevicesOfThisType(QPointer<QTabWidget> parent, DevicesMap& map)
{
    static QProcess process;
    if (process.state() == QProcess::NotRunning)
    {
        if (process.canReadLine())
        {
            QString streamString;
            QTextStream stream;
            stream.setString(&streamString, QIODevice::ReadOnly | QIODevice::Text);
            stream << process.readAll();
            while (!stream.atEnd())
            {
                QString line = stream.readLine();
                if (!line.contains("List of devices attached"))
                {
                    QStringList lineSplit = line.split("\t");
                    if (lineSplit.count() >= 2)
                    {
                        QString deviceId = lineSplit[0];
                        QString deviceStatus = lineSplit[1];
                        qDebug() << "deviceId" << deviceId << "; deviceStatus" << deviceStatus;
                        auto it = map.find(deviceId);
                        if (it == map.end())
                        {
                            map[deviceId] = QSharedPointer<BaseDevice>(
                                new AndroidDevice(parent, deviceId, DeviceType::Android, tr("Android"), deviceStatus)
                            );
                        }
                        else
                        {
                            bool online = deviceStatus == "device";
                            (*it)->setOnline(online);
                        }
                    }
                }
            }
            process.close();
        }

        QStringList args;
        args.append("devices");
        process.start("adb", args);
        process.setReadChannel(QProcess::StandardOutput);
    }
}

qint64 AndroidDevice::readData(char* data, qint64 maxlen)
{
    return 0;
}

qint64 AndroidDevice::writeData(const char* data, qint64 len)
{
    return 0;
}
