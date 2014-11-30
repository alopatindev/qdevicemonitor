#include "AndroidDevice.h"
#include "Utils.h"

#include <QDebug>
#include <QRegExp>
#include <QRegExpValidator>
#include <QScrollBar>
#include <QWeakPointer>

using namespace DataTypes;

static QProcess s_devicesListProcess;

AndroidDevice::AndroidDevice(QPointer<QTabWidget> parent, const QString& id, DeviceType type,
                             const QString& humanReadableName, const QString& humanReadableDescription, QPointer<DeviceAdapter> deviceAdapter)
    : BaseDevice(parent, id, type, humanReadableName, humanReadableDescription, deviceAdapter)
    , m_emptyTextEdit(true)
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
    //m_deviceLogFileStream->flush();
    m_deviceLogFileStream.clear();
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
    m_deviceLogFile.setFileName(
        Utils::getNewLogFilePath("Android-" + Utils::removeSpecialCharacters(m_humanReadableName) + "-")
    );
    m_deviceLogFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    m_deviceLogFileStream = QSharedPointer<QTextStream>(new QTextStream(&m_deviceLogFile));
    m_deviceLogFileStream->setCodec("UTF-8");

    QStringList args;
    args.append("-s");
    args.append(m_id);
    args.append("logcat");
    args.append("-v");
    args.append("threadtime");
    args.append("*:v");
    m_deviceLogProcess.start("adb", args);
    m_deviceLogProcess.setReadChannel(QProcess::StandardOutput);
}

void AndroidDevice::update()
{
    if (m_deviceInfoProcess.state() == QProcess::NotRunning && m_deviceInfoProcess.canReadLine())
    {
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
        QString stringStream;
        QTextStream stream;
        stream.setCodec("UTF-8");
        stream.setString(&stringStream, QIODevice::ReadOnly | QIODevice::Text);
        stream << m_deviceLogProcess.readAll();

        while (!stream.atEnd())
        {
            QString line = stream.readLine();
            *m_deviceLogFileStream << line << "\n";

            static QRegExp rx("([\\d-]+) ([\\d:\\.]+) (\\d+) (\\d+) ([A-Z]) (.+):", Qt::CaseSensitive, QRegExp::W3CXmlSchema11);
            rx.setMinimal(true);

            if (rx.indexIn(line) > -1)
            {
                QString date = rx.cap(1);
                QString time = rx.cap(2);
                QString pid = rx.cap(3);
                QString tid = rx.cap(4);
                QString verbosity = rx.cap(5);
                QString tag = rx.cap(6).trimmed();
                QString text = &(line.toStdString().c_str()[rx.pos(6) + rx.cap(6).toStdString().size() + 2]); // FIXME
                //qDebug() << "date" << date << "time" << time << "pid" << pid << "tid" << tid << "level" << verbosity << "tag" << tag << "text" << text;

                if (Utils::verbosityCharacterToInt(verbosity.toStdString()[0]) <= m_deviceWidget->getVerbosityLevel())
                {
                    m_deviceWidget->getTextEdit().append(line);
                }
            }
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
