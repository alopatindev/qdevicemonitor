#include "AndroidDevice.h"
#include <QTextStream>
#include <QDebug>

using namespace DataTypes;

AndroidDevice::AndroidDevice(QObject* parent, const QString& id, DeviceType type, const QString& humanReadableName, const QString& humanReadableDescription)
    : BaseDevice(parent, id, type, humanReadableName, humanReadableDescription)
{
    updateDeviceModel();
}

void AndroidDevice::updateDeviceModel()
{
    QStringList args;
    args.append("-s");
    args.append(m_id);
    args.append("shell");
    args.append("getprop");
    args.append("ro.product.model");
    deviceInfoProcess.start("adb", args);
    deviceInfoProcess.setReadChannel(QProcess::StandardOutput);
}

void AndroidDevice::update()
{
    if (deviceInfoProcess.state() == QProcess::NotRunning && deviceInfoProcess.canReadLine())
    {
        QString streamString;
        QTextStream stream;
        QString model = deviceInfoProcess.readLine().trimmed();
        qDebug() << "model of" << m_id << "is" << model;
        m_humanReadableName = model;
        updateTabWidget();
    }
}

void AndroidDevice::addNewDevicesOfThisType(QObject* parent, DevicesMap& map)
{
    static QProcess process;
    if (process.state() == QProcess::NotRunning)
    {
        if (process.canReadLine())
        {
            QString streamString;
            QTextStream stream;
            stream.setString(&streamString, QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text);
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
                                new AndroidDevice(parent, deviceId, DeviceType::Android, tr("Unknown Android"), deviceStatus)
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
