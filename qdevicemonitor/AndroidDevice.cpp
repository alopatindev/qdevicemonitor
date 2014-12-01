#include "AndroidDevice.h"
#include "Utils.h"
#include "ThemeColors.h"

#include <QDebug>
#include <QRegExp>
#include <QRegExpValidator>
#include <QScrollBar>
#include <QWeakPointer>

using namespace DataTypes;

static QProcess s_devicesListProcess;
static const char* PLATFORM_STRING = "Android";

AndroidDevice::AndroidDevice(QPointer<QTabWidget> parent, const QString& id, DeviceType type,
                             const QString& humanReadableName, const QString& humanReadableDescription, QPointer<DeviceAdapter> deviceAdapter)
    : BaseDevice(parent, id, type, humanReadableName, humanReadableDescription, deviceAdapter)
    , m_emptyTextEdit(true)
    , m_lastVerbosityLevel(m_deviceWidget->getVerbosityLevel())
    , m_didReadModel(false)
{
    updateDeviceModel();
    startLogger();
}

AndroidDevice::~AndroidDevice()
{
    qDebug() << "~AndroidDevice";
    stopLogger();
    m_deviceInfoProcess.close();
}

void AndroidDevice::updateDeviceModel()
{
    qDebug() << "updateDeviceModel" << m_id;
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

void AndroidDevice::stopLogger()
{
    m_deviceLogProcess.close();
    //m_deviceLogFileStream->flush();
    m_deviceLogFileStream.clear();
    m_deviceLogFile.close();
}

void AndroidDevice::update()
{
    if (!m_didReadModel && m_deviceInfoProcess.state() == QProcess::NotRunning)
    {
        if (m_deviceInfoProcess.canReadLine())
        {
            QString model = m_deviceInfoProcess.readLine().trimmed();
            if (!model.isEmpty())
            {
                qDebug() << "updateDeviceModel" << m_id << "=>" << model;
                m_humanReadableName = model;
                updateTabWidget();
                m_didReadModel = true;
            }
        }

        m_deviceInfoProcess.close();

        if (!m_didReadModel)
        {
            updateDeviceModel();
        }
    }

    if (m_deviceLogProcess.state() == QProcess::Running)
    {
        if (m_deviceWidget->getVerbosityLevel() != m_lastVerbosityLevel)
        {
            m_lastVerbosityLevel = m_deviceWidget->getVerbosityLevel();
            reloadTextEdit();
        }
        else if(m_deviceLogProcess.canReadLine())
        {
            QString stringStream;
            QTextStream stream;
            stream.setCodec("UTF-8");
            stream.setString(&stringStream, QIODevice::ReadOnly | QIODevice::Text);
            //m_deviceWidget->getTextEdit().setPlainText(m_deviceLogProcess.readAll());
            stream << m_deviceLogProcess.readAll();

            while (!stream.atEnd())
            {
                QString line = stream.readLine();
                *m_deviceLogFileStream << line << "\n";
                filterAndAddToTextEdit(line);
            }
        }
    }
}

void AndroidDevice::filterAndAddToTextEdit(const QString& line)
{
    m_deviceWidget->getTextEdit().insertPlainText(line + "\n");
    static QRegExp rx("([\\d-]+) *([\\d:\\.]+) *(\\d+) *(\\d+) *([A-Z]) *(.+):", Qt::CaseSensitive, QRegExp::W3CXmlSchema11);
    rx.setMinimal(true);

    int theme = m_deviceAdapter->isDarkTheme() ? 1 : 0;
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

        int verbosityLevel = Utils::verbosityCharacterToInt(verbosity.toStdString()[0]);
        if (verbosityLevel <= m_deviceWidget->getVerbosityLevel())
        {
            QColor verbosityColor = ThemeColors::Colors[theme][verbosityLevel];

            //m_deviceWidget->getTextEdit().append(line);
            m_deviceWidget->getTextEdit().setTextColor(verbosityColor);
            m_deviceWidget->getTextEdit().insertPlainText(verbosity + " ");

            m_deviceWidget->getTextEdit().setTextColor(QColor(ThemeColors::Colors[theme][ThemeColors::DateTime]));
            m_deviceWidget->getTextEdit().insertPlainText(date + " ");
            m_deviceWidget->getTextEdit().insertPlainText(time + " ");

            m_deviceWidget->getTextEdit().setTextColor(ThemeColors::Colors[theme][ThemeColors::Pid]);
            m_deviceWidget->getTextEdit().insertPlainText(pid + " ");
            m_deviceWidget->getTextEdit().setTextColor(ThemeColors::Colors[theme][ThemeColors::Tid]);
            m_deviceWidget->getTextEdit().insertPlainText(tid + " ");

            m_deviceWidget->getTextEdit().setTextColor(ThemeColors::Colors[theme][ThemeColors::Tag]);
            m_deviceWidget->getTextEdit().insertPlainText(tag + " ");

            m_deviceWidget->getTextEdit().setTextColor(verbosityColor);
            m_deviceWidget->getTextEdit().insertPlainText(text + "\n");
        }

        QPointer<QScrollBar> sb = m_deviceWidget->getTextEdit().verticalScrollBar();
        if (sb->maximum() > 0)
        {
            bool autoScrolling = sb->maximum() < 500 || (sb->value() * 100) / sb->maximum() > 25;  // FIXME: magic number
            if (autoScrolling)
            {
                sb->setValue(sb->maximum());
            }
        }
    }
    else
    {
        qDebug() << "failed to parse" << line;
        m_deviceWidget->getTextEdit().setTextColor(ThemeColors::Colors[theme][ThemeColors::VerbosityVerbose]);
        m_deviceWidget->getTextEdit().insertPlainText(line + "\n");
    }
}

void AndroidDevice::reloadTextEdit()
{
    qDebug() << "reloadTextEdit";
    stopLogger();
    m_deviceWidget->getTextEdit().clear();
    startLogger();
}

void AndroidDevice::addNewDevicesOfThisType(QPointer<QTabWidget> parent, DevicesMap& map, QPointer<DeviceAdapter> deviceAdapter)
{
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
                                new AndroidDevice(parent, deviceId, DeviceType::Android, tr(PLATFORM_STRING), tr("Initializing..."), deviceAdapter)
                            );
                        }
                        else
                        {
                            bool online = deviceStatus == "device";
                            (*it)->setOnline(online);
                            (*it)->setHumanReadableDescription(
                                tr("%1\nStatus: %2\nID: %3%4")
                                    .arg(tr(PLATFORM_STRING))
                                    .arg(online ? "Online" : "Offline")
                                    .arg(deviceId)
                                    .arg(!online ? "\n" + deviceStatus : "")
                            );
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

void AndroidDevice::stopDevicesListProcess()
{
    s_devicesListProcess.close();
}
