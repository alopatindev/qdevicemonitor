#include "AndroidDevice.h"
#include "Utils.h"

#include <QTextStream>
#include <QDebug>

using namespace DataTypes;

static QProcess s_devicesListProcess;

AndroidDevice::AndroidDevice(QPointer<QTabWidget> parent, const QString& id, DeviceType type,
                             const QString& humanReadableName, const QString& humanReadableDescription, QPointer<DeviceAdapter> deviceAdapter)
    : BaseDevice(parent, id, type, humanReadableName, humanReadableDescription, deviceAdapter)
{
    updateDeviceModel();
    startLogger();
}

AndroidDevice::~AndroidDevice()
{
    qDebug() << "~AndroidDevice";
    m_deviceInfoProcess.close();
    m_deviceLogProcess.close();
    s_devicesListProcess.close();
    m_deviceLogFile.flush();
    m_deviceLogFile.close();
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
        QString stringStream;
        QTextStream stream;
        stream.setCodec("UTF-8");
        QString model = m_deviceInfoProcess.readLine().trimmed();
        m_deviceInfoProcess.close();
        //qDebug() << "model of" << m_id << "is" << model;
        if (!model.isEmpty())
        {
            m_humanReadableName = model;
            updateTabWidget();
        }
    }

    if (m_deviceLogProcess.state() == QProcess::Running && m_deviceLogProcess.canReadLine())
    {
        if (!m_deviceLogFile.isOpen())
        {
            m_deviceLogFile.setFileName(
                Utils::getNewLogFilePath("Android-" + Utils::removeSpecialCharacters(m_humanReadableName) + "-")
            );
            m_deviceLogFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
        }

        QString stringStream;
        QTextStream stream;
        stream.setCodec("UTF-8");
        stream.setString(&stringStream, QIODevice::ReadOnly | QIODevice::Text);
        stream << m_deviceLogProcess.readAll();

        QTextStream logFileStream(&m_deviceLogFile);
        logFileStream.setCodec("UTF-8");
        while (!stream.atEnd())
        {
            QString line = stream.readLine();
            m_deviceWidget->getTextEdit().append(line);
            logFileStream << line << "\n";
        }
    }
}

void AndroidDevice::addNewDevicesOfThisType(QPointer<QTabWidget> parent, DevicesMap& map, QPointer<DeviceAdapter> deviceAdapter)
{
    static QProcess s_devicesListProcess;
    if (s_devicesListProcess.state() == QProcess::NotRunning)
    {
        if (s_devicesListProcess.canReadLine())
        {
            QString stringStream;
            QTextStream stream;
            stream.setCodec("UTF-8");
            stream.setString(&stringStream, QIODevice::ReadOnly | QIODevice::Text);
            stream << s_devicesListProcess.readAll();
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
                        //qDebug() << "deviceId" << deviceId << "; deviceStatus" << deviceStatus;
                        auto it = map.find(deviceId);
                        if (it == map.end())
                        {
                            map[deviceId] = QSharedPointer<BaseDevice>(
                                new AndroidDevice(parent, deviceId, DeviceType::Android, tr("Android"), deviceStatus, deviceAdapter)
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
            s_devicesListProcess.close();
        }

        QStringList args;
        args.append("devices");
        s_devicesListProcess.start("adb", args);
        s_devicesListProcess.setReadChannel(QProcess::StandardOutput);
    }
}
