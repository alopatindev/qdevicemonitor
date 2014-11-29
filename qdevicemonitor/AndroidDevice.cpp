#include "AndroidDevice.h"
#include <QProcess>
#include <QTextStream>
#include <QDebug>

using namespace DataTypes;

AndroidDevice::AndroidDevice(const QString& id, DeviceType type, const QString& humanReadableName, const QString& humanReadableDescription)
    : BaseDevice(id, type, humanReadableName, humanReadableDescription)
{
}

void AndroidDevice::addNewDevicesOfThisType(DevicesMap& map)
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
                        if (map.find(deviceId) == map.end())
                        {
                            map[deviceId] = QSharedPointer<BaseDevice>(new AndroidDevice(deviceId, DeviceType::Android, tr("Unknown Android"), deviceStatus));
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
