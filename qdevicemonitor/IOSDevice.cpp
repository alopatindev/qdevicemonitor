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

#include "IOSDevice.h"
#include "Utils.h"
#include "ThemeColors.h"

#include <QDebug>
#include <QFileInfo>
#include <QHash>
#include <QRegularExpression>

using namespace DataTypes;

static QProcess s_devicesListProcess;
static QHash<QString, bool> s_removedDeviceByTabClose;

IOSDevice::IOSDevice(QPointer<QTabWidget> parent, const QString& id, const DeviceType type,
                     const QString& humanReadableName, const QString& humanReadableDescription, QPointer<DeviceAdapter> deviceAdapter)
    : BaseDevice(parent, id, type, humanReadableName, humanReadableDescription, deviceAdapter)
    , m_didReadDeviceModel(false)
{
    qDebug() << "IOSDevice::IOSDevice";
    m_deviceWidget->getFilterLineEdit().setToolTip(tr("Search for messages. Accepts<ul><li>Plain Text</li><li>Prefix <b>text:</b> with Plain Text</li><li>Regular Expressions</li></ul>"));
    m_deviceWidget->hideVerbosity();

    updateDeviceModel();
    connect(&m_deviceInfoProcess, &QProcess::readyReadStandardError, this, &IOSDevice::readStandardError);
}

IOSDevice::~IOSDevice()
{
    qDebug() << "IOSDevice::~IOSDevice";
    stopLogger();
    disconnect(&m_deviceInfoProcess, nullptr, this, nullptr);
    m_deviceInfoProcess.terminate();
    m_deviceInfoProcess.close();
}

void IOSDevice::updateDeviceModel()
{
    qDebug() << "updateDeviceModel" << m_id;
    QStringList args;
    args.append("-u");
    args.append(m_id);
    args.append("-s");
    args.append("-k");
    args.append("DeviceName");
    m_deviceInfoProcess.setReadChannel(QProcess::StandardOutput);
    m_deviceInfoProcess.start("ideviceinfo", args);
}

void IOSDevice::startLogger()
{
    if (!m_didReadDeviceModel)
    {
        return;
    }

    qDebug() << "IOSDevice::startLogger";

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
    args.append("-u");
    args.append(m_id);
    m_deviceLogProcess.setReadChannel(QProcess::StandardOutput);
    m_deviceLogProcess.start("idevicesyslog", args);
}

void IOSDevice::stopLogger()
{
    qDebug() << "IOSDevice::stopLogger";

    m_deviceLogProcess.terminate();
    m_deviceLogProcess.close();
    m_deviceLogFileStream.clear();
    m_deviceLogFile.close();
}

void IOSDevice::update()
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
            if (m_lastFilter != filter)
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

void IOSDevice::checkFilters(bool& filtersMatch, bool& filtersValid, const QStringList& filters, const QStringRef& text)
{
    for (auto& filter : filters)
    {
        bool columnFound = false;
        if (!columnMatches("text:", filter, text, filtersValid, columnFound))
        {
            filtersMatch = false;
            break;
        }

        if (!columnFound && !columnTextMatches(filter, text))
        {
            filtersMatch = false;
            break;
        }
    }
}

void IOSDevice::filterAndAddToTextEdit(const QString& line)
{
    if (line == QString("[connected]") || line == QString("[disconnected]"))
    {
        return;
    }

    static const QRegularExpression re(
        "(?<prefix>[A-Za-z]* +[\\d]+ [\\d:]+) (?<deviceName>.+) ",
        QRegularExpression::InvertedGreedinessOption | QRegularExpression::DotMatchesEverythingOption
    );

    const int themeIndex = m_deviceAdapter->isDarkTheme() ? 1 : 0;
    const QRegularExpressionMatch match = re.match(line);
    if (match.hasMatch())
    {
        const QStringRef prefix = match.capturedRef("prefix");
        const QStringRef deviceName = match.capturedRef("deviceName");
        const QStringRef text = line.midRef(match.capturedEnd("hostname") + 1);

        bool filtersMatch = true;
        checkFilters(filtersMatch, m_filtersValid, m_filters, text);

        if (filtersMatch)
        {
            m_deviceWidget->addText(ThemeColors::Colors[themeIndex][ThemeColors::DateTime], prefix);
            m_deviceWidget->addText(ThemeColors::Colors[themeIndex][ThemeColors::VerbosityWarn], deviceName);
            m_deviceWidget->addText(ThemeColors::Colors[themeIndex][ThemeColors::VerbosityVerbose], text);
            m_deviceWidget->flushText();
        }
    }
    else
    {
        bool filtersMatch = true;
        checkFilters(filtersMatch, m_filtersValid, m_filters, QStringRef(&line));

        if (filtersMatch)
        {
            m_deviceWidget->addText(ThemeColors::Colors[themeIndex][ThemeColors::VerbosityInfo], QStringRef(&line));
            m_deviceWidget->flushText();
        }
    }

    m_deviceWidget->maybeScrollTextEditToEnd();
    m_deviceWidget->highlightFilterLineEdit(!m_filtersValid);
}

void IOSDevice::reloadTextEdit()
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

void IOSDevice::maybeAddNewDevicesOfThisType(QPointer<QTabWidget> parent, DevicesMap& map, QPointer<DeviceAdapter> deviceAdapter)
{
    auto updateDeviceStatus = [](const QString& deviceStatus, BaseDevice& device, const QString& deviceId)
    {
        const bool online = deviceStatus == "device";
        device.setHumanReadableDescription(
            tr("%1\nStatus: %2\nID: %3")
                .arg(tr(getPlatformStringStatic()))
                .arg(online ? "Online" : "Offline")
                .arg(deviceId)
        );
        device.setOnline(online);
        device.setVisited(true);
    };


    if (s_devicesListProcess.state() == QProcess::NotRunning)
    {
        bool deviceListError = false;

        if (s_devicesListProcess.exitCode() != 0 ||
            s_devicesListProcess.exitStatus() == QProcess::ExitStatus::CrashExit)
        {
            deviceListError = true;

            QString stringStream;
            QTextStream stream;
            stream.setCodec("UTF-8");
            stream.setString(&stringStream, QIODevice::ReadWrite | QIODevice::Text);
            stream << s_devicesListProcess.readAllStandardError();
            const QString errorText = stream.readLine();

            if (s_devicesListProcess.exitCode() != 0xFF || errorText != "ERROR: Unable to retrieve device list!")
            {
                qDebug() << "IOSDevice::s_devicesListProcess exitCode" << s_devicesListProcess.exitCode()
                         << "; exitStatus" << s_devicesListProcess.exitStatus()
                         << "; stderr" << errorText;
            }
            else
            {
#if defined(Q_OS_LINUX)
                // TODO: if ps uax | grep usbmuxd
                deviceListError = false;
#endif
            }
        }

        if (!deviceListError)
        {
            for (auto& i : s_removedDeviceByTabClose)
            {
                i = false;  // not visited
            }

            for (auto& dev : map)
            {
                if (dev->getType() == DeviceType::IOS)
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
                    const QString deviceId = stream.readLine();
                    //qDebug() << "deviceId" << deviceId;
                    if (s_removedDeviceByTabClose.contains(deviceId))
                    {
                        s_removedDeviceByTabClose[deviceId] = true;  // visited
                    }
                    else
                    {
                        auto it = map.find(deviceId);
                        if (it == map.end())
                        {
                            map[deviceId] = QSharedPointer<IOSDevice>::create(
                                parent,
                                deviceId,
                                DeviceType::IOS,
                                QString(getPlatformStringStatic()),
                                tr("Initializing..."),
                                deviceAdapter
                            );
                        }
                        else if ((*it)->getType() != DeviceType::IOS)
                        {
                            qDebug() << "id collision";
                        }
                        else
                        {
                            updateDeviceStatus("device", *(*it), deviceId);
                        }
                    }
                }
            }

            for (auto& dev : map)
            {
                if (dev->getType() == DeviceType::IOS)
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
        args.append("-l");
        s_devicesListProcess.setReadChannel(QProcess::StandardOutput);
        s_devicesListProcess.start("idevice_id", args);
    }
}

void IOSDevice::stopDevicesListProcess()
{
    s_devicesListProcess.close();
}

void IOSDevice::removedDeviceByTabClose(const QString& id)
{
    s_removedDeviceByTabClose[id] = false;
}

void IOSDevice::readStandardError()
{
    qDebug() << "readStandardError";
    QString stringStream;
    QTextStream stream;
    stream.setCodec("UTF-8");
    stream.setString(&stringStream, QIODevice::ReadWrite | QIODevice::Text);
    stream << m_deviceInfoProcess.readAllStandardError();

    const int themeIndex = m_deviceAdapter->isDarkTheme() ? 1 : 0;
    for (int i = 0; i < DeviceAdapter::MAX_LINES_UPDATE && !stream.atEnd(); ++i)
    {
        const QString line = stream.readLine();
        m_deviceWidget->addText(ThemeColors::Colors[themeIndex][ThemeColors::VerbosityAssert], QStringRef(&line));
    }
}
