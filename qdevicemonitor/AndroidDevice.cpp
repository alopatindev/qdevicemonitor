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
#include <QRegularExpression>

using namespace DataTypes;

static QSharedPointer<QString> s_tempBuffer;
static QSharedPointer<QTextStream> s_tempStream;

static QProcess s_devicesListProcess;
static QHash<QString, bool> s_removedDeviceByTabClose;

AndroidDevice::AndroidDevice(QPointer<QTabWidget> parent, const QString& id, const DeviceType type,
                             const QString& humanReadableName, const QString& humanReadableDescription, QPointer<DeviceAdapter> deviceAdapter)
    : BaseDevice(parent, id, type, humanReadableName, humanReadableDescription, deviceAdapter)
    , m_lastVerbosityLevel(m_deviceWidget->getVerbosityLevel())
    , m_didReadDeviceModel(false)
{
    qDebug() << "AndroidDevice::AndroidDevice";
    m_deviceWidget->getFilterLineEdit().setToolTip(tr("Search for messages. Accepts<ul><li>Plain Text</li><li>Prefixes (<b>pid:</b>, <b>tid:</b>, <b>tag:</b> or <b>text:</b>) with Plain Text</li><li>Regular Expressions</li></ul>"));
    updateDeviceModel();
}

AndroidDevice::~AndroidDevice()
{
    qDebug() << "AndroidDevice::~AndroidDevice";
    stopLogger();
    stopDeviceInfoProcess();
}

void AndroidDevice::stopDeviceInfoProcess()
{
    if (m_deviceInfoProcess.state() != QProcess::NotRunning)
    {
        m_deviceInfoProcess.terminate();
        m_deviceInfoProcess.kill();
        m_deviceInfoProcess.close();
    }
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
    m_deviceLogFileStream = QSharedPointer<QTextStream>::create(&m_deviceLogFile);
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

    if (m_deviceLogProcess.state() != QProcess::NotRunning)
    {
        m_deviceLogProcess.terminate();
        m_deviceLogProcess.kill();
        m_deviceLogProcess.close();
    }

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
                //stopLogger();
                startLogger();
            }
        }

        stopDeviceInfoProcess();

        if (!m_didReadDeviceModel)
        {
            updateDeviceModel();
        }
    }

    switch (m_deviceLogProcess.state())
    {
    case QProcess::Running:
        {
            if (!isOnline())
            {
                qDebug() << "not updating '" << m_id << "' because it's offline";
                //stopLogger();
            }
            else if (m_deviceWidget->getVerbosityLevel() != m_lastVerbosityLevel)
            {
                m_lastVerbosityLevel = m_deviceWidget->getVerbosityLevel();
                reloadTextEdit();
            }
            else if (m_dirtyFilter)
            {
                m_dirtyFilter = false;
                const QString filter = m_deviceWidget->getFilterLineEdit().text();
                m_filters = filter.split(' ');
                m_filtersValid = true;
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
            else if (m_deviceLogProcess.canReadLine())
            {
                for (int i = 0; i < DeviceAdapter::MAX_LINES_UPDATE && m_deviceLogProcess.canReadLine(); ++i)
                {
                    m_tempStream << m_deviceLogProcess.readLine();
                    const QString line = m_tempStream.readLine();
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
            if (isOnline())
            {
                qDebug() << "m_deviceLogProcess not running; are we still online?";
                //stopLogger();
                //startLogger();
            }
        }
        break;
    case QProcess::Starting:
    default:
        break;
    }
}

void AndroidDevice::filterAndAddToTextEdit(const QString& line)
{
    static const QRegularExpression re(
        "(?<date>[\\d-]+) *(?<time>[\\d:\\.]+) *(?<pid>\\d+) *(?<tid>\\d+) *(?<verbosity>[A-Z]) *(?<tag>.+):",
        QRegularExpression::InvertedGreedinessOption | QRegularExpression::DotMatchesEverythingOption
    );

    bool filtersMatch = true;
    const int themeIndex = m_deviceAdapter->isDarkTheme() ? 1 : 0;

    const QRegularExpressionMatch match = re.match(line);
    if (match.hasMatch())
    {
        const QStringRef date = match.capturedRef("date");
        const QStringRef time = match.capturedRef("time");
        const QStringRef pid = match.capturedRef("pid");
        const QStringRef tid = match.capturedRef("tid");
        const QStringRef verbosity = match.capturedRef("verbosity");
        const QStringRef tag = match.capturedRef("tag").trimmed();
        const QStringRef text = line.midRef(match.capturedEnd("tag") + 1);

        const VerbosityEnum verbosityLevel = static_cast<VerbosityEnum>(Utils::verbosityCharacterToInt(verbosity.at(0).toLatin1()));

        checkFilters(filtersMatch, m_filtersValid, verbosityLevel, pid, tid, tag, text);

        if (filtersMatch)
        {
            const QColor verbosityColor = ThemeColors::Colors[themeIndex][verbosityLevel];

            m_deviceWidget->addText(verbosityColor, verbosity);
            m_deviceWidget->addText(ThemeColors::Colors[themeIndex][ThemeColors::DateTime], date);
            m_deviceWidget->addText(ThemeColors::Colors[themeIndex][ThemeColors::DateTime], time);
            m_deviceWidget->addText(ThemeColors::Colors[themeIndex][ThemeColors::Pid], pid);
            m_deviceWidget->addText(ThemeColors::Colors[themeIndex][ThemeColors::Tid], tid);
            m_deviceWidget->addText(ThemeColors::Colors[themeIndex][ThemeColors::Tag], tag);
            m_deviceWidget->addText(verbosityColor, text);
            m_deviceWidget->flushText();
        }
    }
    else
    {
        qDebug() << "failed to parse" << line;
        checkFilters(filtersMatch, m_filtersValid);
        if (filtersMatch)
        {
            m_deviceWidget->addText(ThemeColors::Colors[themeIndex][ThemeColors::VerbosityVerbose], QStringRef(&line));
            m_deviceWidget->flushText();
        }
    }

    m_deviceWidget->maybeScrollTextEditToEnd();
    m_deviceWidget->highlightFilterLineEdit(!m_filtersValid);
}

void AndroidDevice::checkFilters(bool& filtersMatch, bool& filtersValid, const VerbosityEnum verbosityLevel, const QStringRef& pid, const QStringRef& tid, const QStringRef& tag, const QStringRef& text)
{
    filtersMatch = verbosityLevel <= m_deviceWidget->getVerbosityLevel();

    if (!filtersMatch || !filtersValid)
    {
        return;
    }

    for (auto it = m_filters.constBegin(); it != m_filters.constEnd(); ++it)
    {
        const QStringRef filter(&(*it));
        bool columnFound = false;
        if (!columnMatches("pid:", filter, pid, filtersValid, columnFound) ||
            !columnMatches("tid:", filter, tid, filtersValid, columnFound) ||
            !columnMatches("tag:", filter, tag, filtersValid, columnFound) ||
            !columnMatches("text:", filter, text, filtersValid, columnFound))
        {
            filtersMatch = false;
            break;
        }

        if (!columnFound && !columnTextMatches(filter, text.toString()))
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
        startLogger();
    }
    else
    {
        stopLogger();
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
        m_deviceClearLogProcess.terminate();
        m_deviceClearLogProcess.kill();
        m_deviceClearLogProcess.close();
    }
}

void AndroidDevice::updateDeviceStatus(const QString& deviceStatus, BaseDevice& device, const QString& deviceId)
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
}

void AndroidDevice::maybeAddNewDevicesOfThisType(QPointer<QTabWidget> parent, DevicesMap& map, QPointer<DeviceAdapter> deviceAdapter)
{
    if (s_devicesListProcess.state() == QProcess::NotRunning)
    {
        if (s_tempStream.isNull())
        {
            s_tempStream = QSharedPointer<QTextStream>::create();
            s_tempBuffer = QSharedPointer<QString>::create();
            s_tempStream->setCodec("UTF-8");
            s_tempStream->setString(&(*s_tempBuffer), QIODevice::ReadWrite | QIODevice::Text);
        }

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
                *s_tempStream << s_devicesListProcess.readAll();

                while (!s_tempStream->atEnd())
                {
                    const QString line = s_tempStream->readLine();
                    if (line.contains("List of devices attached"))
                    {
                        continue;
                    }

                    // TODO: replace split with splitRef
                    const QStringList lineSplit = line.split('\t');
                    if (lineSplit.count() >= 2)
                    {
                        const QString& deviceId = lineSplit[0];
                        const QString& deviceStatus = lineSplit[1];
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
                                map[deviceId] = QSharedPointer<AndroidDevice>::create(
                                    parent,
                                    deviceId,
                                    DeviceType::Android,
                                    QString(getPlatformStringStatic()),
                                    tr("Initializing..."),
                                    deviceAdapter
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

        stopDevicesListProcess();

        QStringList args;
        args.append("devices");
        s_devicesListProcess.setReadChannel(QProcess::StandardOutput);
        s_devicesListProcess.start("adb", args);
    }
}

void AndroidDevice::releaseTempBuffer()
{
    qDebug() << "AndroidDevice::releaseTempBuffer";
    if (!s_tempStream.isNull())
    {
        s_tempStream.clear();
        s_tempBuffer.clear();
    }
}

void AndroidDevice::stopDevicesListProcess()
{
    if (s_devicesListProcess.state() != QProcess::NotRunning)
    {
        s_devicesListProcess.terminate();
        s_devicesListProcess.kill();
        s_devicesListProcess.close();
    }
}

void AndroidDevice::removedDeviceByTabClose(const QString& id)
{
    s_removedDeviceByTabClose[id] = false;
}
