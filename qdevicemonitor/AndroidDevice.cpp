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

#include "AndroidDevice.h"
#include "Utils.h"
#include "ThemeColors.h"

#include <QDebug>
#include <QHash>
#include <QRegExp>

using namespace DataTypes;

static QProcess s_devicesListProcess;
static QHash<QString, bool> s_removedDeviceByTabClose;

AndroidDevice::AndroidDevice(QPointer<QTabWidget> parent, const QString& id, const DeviceType type,
                             const QString& humanReadableName, const QString& humanReadableDescription, QPointer<DeviceAdapter> deviceAdapter)
    : BaseDevice(parent, id, type, humanReadableName, humanReadableDescription, deviceAdapter)
    , m_lastVerbosityLevel(m_deviceWidget->getVerbosityLevel())
    , m_didReadDeviceModel(false)
{
    qDebug() << "AndroidDevice::AndroidDevice";
    updateDeviceModel();
}

AndroidDevice::~AndroidDevice()
{
    qDebug() << "AndroidDevice::~AndroidDevice";
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
    m_deviceInfoProcess.setReadChannel(QProcess::StandardOutput);
    m_deviceInfoProcess.start("adb", args);
}

void AndroidDevice::startLogger()
{
    if (!m_didReadDeviceModel)
    {
        return;
    }

    qDebug() << "AndroidDevice::startLogger";

    const QString currentLogAbsFileName = Utils::getNewLogFilePath(
        QString("%1-%2-")
            .arg(getPlatformStringStatic())
            .arg(Utils::removeSpecialCharacters(m_humanReadableName))
    );
    m_currentLogFileName = currentLogAbsFileName;
    m_deviceWidget->onLogFileNameChanged(m_currentLogFileName);

    m_deviceLogFile.setFileName(currentLogAbsFileName);
    m_deviceLogFile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
    m_deviceLogFileStream = QSharedPointer<QTextStream>(new QTextStream(&m_deviceLogFile));
    m_deviceLogFileStream->setCodec("UTF-8");

    QStringList args;
    args.append("-s");
    args.append(m_id);
    args.append("logcat");
    args.append("-v");
    args.append("threadtime");
    args.append("*:v");
    m_deviceLogProcess.setReadChannel(QProcess::StandardOutput);
    m_deviceLogProcess.start("adb", args);
}

void AndroidDevice::stopLogger()
{
    qDebug() << "AndroidDevice::stopLogger";

    m_deviceLogProcess.close();
    m_deviceLogFileStream.clear();
    m_deviceLogFile.close();
}

void AndroidDevice::update()
{
    if (!m_didReadDeviceModel && m_deviceInfoProcess.state() == QProcess::NotRunning)
    {
        if (m_deviceInfoProcess.canReadLine())
        {
            const QString model = m_deviceInfoProcess.readLine().trimmed();
            if (!model.isEmpty())
            {
                qDebug() << "updateDeviceModel" << m_id << "=>" << model;
                m_humanReadableName = model;
                updateTabWidget();
                m_didReadDeviceModel = true;
                stopLogger();
                startLogger();
            }
        }

        m_deviceInfoProcess.close();

        if (!m_didReadDeviceModel)
        {
            updateDeviceModel();
        }
    }

    switch (m_deviceLogProcess.state())
    {
    case QProcess::Running:
        {
            const QString filter = m_deviceWidget->getFilterLineEdit().text();
            if (m_deviceWidget->getVerbosityLevel() != m_lastVerbosityLevel)
            {
                m_lastVerbosityLevel = m_deviceWidget->getVerbosityLevel();
                reloadTextEdit();
            }
            else if (m_lastFilter != filter)
            {
                m_filters = filter.split(" ");
                m_filtersValid = true;
                m_lastFilter = filter;
                reloadTextEdit();
                maybeAddCompletionAfterDelay(filter);
            }
            else if (!m_deviceLogFileStream->atEnd())
            {
                for (int i = 0; i < DeviceAdapter::MAX_LINES_UPDATE && !m_deviceLogFileStream->atEnd(); ++i)
                {
                    filterAndAddToTextEdit(m_deviceLogFileStream->readLine());
                }
            }
            else if(m_deviceLogProcess.canReadLine())
            {
                QString stringStream;
                QTextStream stream;
                stream.setCodec("UTF-8");
                stream.setString(&stringStream, QIODevice::ReadWrite | QIODevice::Text);

                for (int i = 0; i < DeviceAdapter::MAX_LINES_UPDATE && m_deviceLogProcess.canReadLine(); ++i)
                {
                    stream << m_deviceLogProcess.readLine();
                    const QString line = stream.readLine();
                    *m_deviceLogFileStream << line << "\n";
                    m_deviceLogFileStream->flush();
                    addToLogBuffer(line);
                    filterAndAddToTextEdit(line);
                }
            }
        }
        break;
    case QProcess::NotRunning:
        {
            qDebug() << "m_deviceLogProcess not running";
            stopLogger();
            startLogger();
        }
        break;
    case QProcess::Starting:
    default:
        break;
    }
}

void AndroidDevice::filterAndAddToTextEdit(const QString& line)
{
    QRegExp rx("([\\d-]+) *([\\d:\\.]+) *(\\d+) *(\\d+) *([A-Z]) *(.+):", Qt::CaseSensitive, QRegExp::W3CXmlSchema11);
    rx.setMinimal(true);

    bool filtersMatch = true;

    int theme = m_deviceAdapter->isDarkTheme() ? 1 : 0;
    if (rx.indexIn(line) > -1)
    {
        const QString date = rx.cap(1);
        const QString time = rx.cap(2);
        const QString pid = rx.cap(3);
        const QString tid = rx.cap(4);
        const QString verbosity = rx.cap(5);
        const QString tag = rx.cap(6).trimmed();
        const QString text = line.mid(rx.pos(6) + rx.cap(6).length() + 2); // the rest of the line after "foo: "
        //qDebug() << "date" << date << "time" << time << "pid" << pid << "tid" << tid << "level" << verbosity << "tag" << tag << "text" << text;

        const VerbosityEnum verbosityLevel = static_cast<VerbosityEnum>(Utils::verbosityCharacterToInt(verbosity[0].toLatin1()));
        checkFilters(filtersMatch, m_filtersValid, m_filters, verbosityLevel, pid, tid, tag, text);

        if (filtersMatch)
        {
            const QColor verbosityColor = ThemeColors::Colors[theme][verbosityLevel];

            m_deviceWidget->addText(verbosityColor, verbosity + " ");
            m_deviceWidget->addText(ThemeColors::Colors[theme][ThemeColors::DateTime], date + " " + time + " ");
            m_deviceWidget->addText(ThemeColors::Colors[theme][ThemeColors::Pid], pid + " ");
            m_deviceWidget->addText(ThemeColors::Colors[theme][ThemeColors::Tid], tid + " ");
            m_deviceWidget->addText(ThemeColors::Colors[theme][ThemeColors::Tag], tag + " ");
            m_deviceWidget->addText(verbosityColor, text + "\n");
        }
    }
    else
    {
        qDebug() << "failed to parse" << line;
        checkFilters(filtersMatch, m_filtersValid, m_filters);
        if (filtersMatch)
        {
            m_deviceWidget->addText(ThemeColors::Colors[theme][ThemeColors::VerbosityVerbose], line + "\n");
        }
    }

    m_deviceWidget->maybeScrollTextEditToEnd();
    m_deviceWidget->highlightFilterLineEdit(!m_filtersValid);
}

void AndroidDevice::checkFilters(bool& filtersMatch, bool& filtersValid, const QStringList& filters, VerbosityEnum verbosityLevel, const QString& pid, const QString& tid, const QString& tag, const QString& text) const
{
    filtersMatch = verbosityLevel <= m_deviceWidget->getVerbosityLevel();

    if (!filtersMatch || !filtersValid)
    {
        return;
    }

    for (const auto& filter : filters)
    {
        bool columnFound = false;
        if (!Utils::columnMatches("pid:", filter, pid, filtersValid, columnFound) ||
            !Utils::columnMatches("tid:", filter, tid, filtersValid, columnFound) ||
            !Utils::columnMatches("tag:", filter, tag, filtersValid, columnFound) ||
            !Utils::columnMatches("text:", filter, text, filtersValid, columnFound))
        {
            filtersMatch = false;
            break;
        }

        if (!columnFound && !Utils::columnTextMatches(filter, text))
        {
            filtersMatch = false;
            break;
        }
    }
}

void AndroidDevice::reloadTextEdit()
{
    if (!m_didReadDeviceModel)
    {
        return;
    }

    qDebug() << "reloadTextEdit";
    m_deviceWidget->clearTextEdit();

    updateLogBufferSpace();
    filterAndAddFromLogBufferToTextEdit();
}

void AndroidDevice::onOnlineChange(const bool online)
{
    if (online)
    {
        maybeClearAdbLog();
    }
}

void AndroidDevice::maybeClearAdbLog()
{
    if (m_deviceAdapter->getClearAndroidLog() && isOnline())
    {
        qDebug() << "clearing adb log";

        QStringList args;
        args.append("-s");
        args.append(m_id);
        args.append("logcat");
        args.append("-c");
        m_deviceClearLogProcess.setReadChannel(QProcess::StandardOutput);
        m_deviceClearLogProcess.start("adb", args);
        m_deviceClearLogProcess.waitForFinished(1000);
        m_deviceClearLogProcess.close();
    }
}

void AndroidDevice::maybeAddNewDevicesOfThisType(QPointer<QTabWidget> parent, DevicesMap& map, QPointer<DeviceAdapter> deviceAdapter)
{
    auto updateDeviceStatus = [](const QString& deviceStatus, BaseDevice& device, const QString& deviceId)
    {
        const bool online = deviceStatus == "device";
        device.setHumanReadableDescription(
            tr("%1\nStatus: %2\nID: %3%4")
                .arg(tr(getPlatformStringStatic()))
                .arg(online ? "Online" : "Offline")
                .arg(deviceId)
                .arg(!online && !deviceStatus.isEmpty() ? "\n" + deviceStatus : "")
        );
        device.setOnline(online);
        device.setVisited(true);
    };

    if (s_devicesListProcess.state() == QProcess::NotRunning)
    {
        if (s_devicesListProcess.exitCode() != 0 ||
            s_devicesListProcess.exitStatus() == QProcess::ExitStatus::CrashExit)
        {
            qDebug() << "AndroidDevice::s_devicesListProcess exitCode" << s_devicesListProcess.exitCode()
                     << "; exitStatus" << s_devicesListProcess.exitStatus()
                     << "; stderr" << s_devicesListProcess.readAllStandardError();
        }
        else
        {
            for (auto& i : s_removedDeviceByTabClose)
            {
                i = false;  // not visited
            }

            for (auto& dev : map)
            {
                if (dev->getType() == DeviceType::Android)
                {
                    dev->setVisited(false);
                }
            }

            if (s_devicesListProcess.canReadLine())
            {
                QString stringStream;
                QTextStream stream;
                stream.setCodec("UTF-8");
                stream.setString(&stringStream, QIODevice::ReadWrite | QIODevice::Text);
                stream << s_devicesListProcess.readAll();

                while (!stream.atEnd())
                {
                    const QString line = stream.readLine();
                    if (!line.contains("List of devices attached"))
                    {
                        const QStringList lineSplit = line.split("\t");
                        if (lineSplit.count() >= 2)
                        {
                            const QString deviceId = lineSplit[0];
                            const QString deviceStatus = lineSplit[1];
                            //qDebug() << "deviceId" << deviceId << "; deviceStatus" << deviceStatus;
                            if (s_removedDeviceByTabClose.contains(deviceId))
                            {
                                s_removedDeviceByTabClose[deviceId] = true;  // visited
                            }
                            else
                            {
                                auto it = map.find(deviceId);
                                if (it == map.end())
                                {
                                    map[deviceId] = QSharedPointer<BaseDevice>(
                                        new AndroidDevice(
                                            parent,
                                            deviceId,
                                            DeviceType::Android,
                                            QString(getPlatformStringStatic()),
                                            tr("Initializing..."),
                                            deviceAdapter
                                        )
                                    );
                                }
                                else if ((*it)->getType() != DeviceType::Android)
                                {
                                    qDebug() << "id collision";
                                }
                                else
                                {
                                    updateDeviceStatus(deviceStatus, *(*it), deviceId);
                                }
                            }
                        }
                    }
                }
            }

            for (auto& dev : map)
            {
                if (dev->getType() == DeviceType::Android)
                {
                    if (!dev->isVisited())
                    {
                        if (!s_removedDeviceByTabClose.contains(dev->getId()))
                        {
                            updateDeviceStatus("", *dev, dev->getId());
                        }
                    }
                }
            }

            for (auto it = s_removedDeviceByTabClose.begin(); it != s_removedDeviceByTabClose.end(); )
            {
                const bool becameOffline = it.value() == false;
                if (becameOffline)
                {
                    it = s_removedDeviceByTabClose.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

        s_devicesListProcess.close();

        QStringList args;
        args.append("devices");
        s_devicesListProcess.setReadChannel(QProcess::StandardOutput);
        s_devicesListProcess.start("adb", args);
    }
}

void AndroidDevice::stopDevicesListProcess()
{
    s_devicesListProcess.close();
}

void AndroidDevice::removedDeviceByTabClose(const QString& id)
{
    s_removedDeviceByTabClose[id] = false;
}
