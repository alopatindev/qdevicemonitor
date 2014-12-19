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
#include <QFileInfo>
#include <QHash>
#include <QRegExp>
#include <QWeakPointer>

using namespace DataTypes;

static QProcess s_devicesListProcess;
static QHash<QString, bool> s_removedDeviceByTabClose;

AndroidDevice::AndroidDevice(QPointer<QTabWidget> parent, const QString& id, DeviceType type,
                             const QString& humanReadableName, const QString& humanReadableDescription, QPointer<DeviceAdapter> deviceAdapter)
    : BaseDevice(parent, id, type, humanReadableName, humanReadableDescription, deviceAdapter)
    , m_emptyTextEdit(true)
    , m_lastVerbosityLevel(m_deviceWidget->getVerbosityLevel())
    , m_didReadDeviceModel(false)
    , m_filtersValid(true)
{
    updateDeviceModel();
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

    const QString currentLogAbsFileName = Utils::getNewLogFilePath("Android-" + Utils::removeSpecialCharacters(m_humanReadableName) + "-");
    m_currentLogFileName = QFileInfo(currentLogAbsFileName).fileName();

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
    //m_deviceLogFileStream->flush();
    m_deviceLogFileStream.clear();
    m_deviceLogFile.close();
}

void AndroidDevice::update()
{
    if (!m_didReadDeviceModel && m_deviceInfoProcess.state() == QProcess::NotRunning)
    {
        if (m_deviceInfoProcess.canReadLine())
        {
            QString model = m_deviceInfoProcess.readLine().trimmed();
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
                scheduleReloadTextEdit();
            }
            else if (m_lastFilter.compare(filter) != 0)
            {
                m_filters = filter.split(" ");
                m_filtersValid = true;
                m_lastFilter = filter;
                scheduleReloadTextEdit();
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

                for (int i = 0; i < DeviceAdapter::MAX_LINES_UPDATE && !m_deviceLogProcess.atEnd(); ++i)
                {
                    stream << m_deviceLogProcess.readLine();
                    QString line = stream.readLine();
                    *m_deviceLogFileStream << line << "\n";
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
    static QRegExp rx("([\\d-]+) *([\\d:\\.]+) *(\\d+) *(\\d+) *([A-Z]) *(.+):", Qt::CaseSensitive, QRegExp::W3CXmlSchema11);
    rx.setMinimal(true);

    bool filtersMatch = true;

    int theme = m_deviceAdapter->isDarkTheme() ? 1 : 0;
    if (rx.indexIn(line) > -1)
    {
        QString date = rx.cap(1);
        QString time = rx.cap(2);
        QString pid = rx.cap(3);
        QString tid = rx.cap(4);
        QString verbosity = rx.cap(5);
        QString tag = rx.cap(6).trimmed();
        QString text = line.mid(rx.pos(6) + rx.cap(6).length() + 2); // the rest of the line after "foo: "
        //qDebug() << "date" << date << "time" << time << "pid" << pid << "tid" << tid << "level" << verbosity << "tag" << tag << "text" << text;

        VerbosityEnum verbosityLevel = static_cast<VerbosityEnum>(Utils::verbosityCharacterToInt(verbosity[0].toLatin1()));
        checkFilters(filtersMatch, m_filtersValid, m_filters, verbosityLevel, pid, tid, tag, text);

        if (filtersMatch)
        {
            QColor verbosityColor = ThemeColors::Colors[theme][verbosityLevel];

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

bool AndroidDevice::columnMatches(const QString& column, const QString& filter, const QString& originalValue, bool& filtersValid, bool& columnFound) const
{
    if (filter.startsWith(column))
    {
        columnFound = true;
        QString value = filter.mid(column.length());
        if (value.isEmpty())
        {
            filtersValid = false;
        }
        else if (!originalValue.startsWith(value))
        {
            return false;
        }
    }
    return true;
}

bool AndroidDevice::columnTextMatches(const QString& filter, const QString& text) const
{
    static QString f[3];
    f[0] = filter.trimmed();

    if (f[0].isEmpty() || text.indexOf(f[0]) != -1)
    {
        return true;
    }
    else
    {
        f[1] = QString(".*%1.*").arg(f[0]);
        f[2] = QString(".*(%1).*").arg(f[1]);

        static QRegExp rx[] = {
            //QRegExp("", Qt::CaseSensitive, QRegExp::RegExp),
            QRegExp("", Qt::CaseSensitive, QRegExp::RegExp2),
            QRegExp("", Qt::CaseSensitive, QRegExp::Wildcard),
            QRegExp("", Qt::CaseSensitive, QRegExp::WildcardUnix),
            //QRegExp("", Qt::CaseSensitive, QRegExp::FixedString),
            QRegExp("", Qt::CaseSensitive, QRegExp::W3CXmlSchema11)
        };

        for (size_t i = 0; i < sizeof(rx) / sizeof(rx[0]); ++i)
        {
            QRegExp& r = rx[i];
            for (size_t j = 0; j < sizeof(f) / sizeof(f[0]); ++j)
            {
                r.setPattern(f[j]);
                if (r.isValid() && r.exactMatch(text))
                {
                    return true;
                }
            }
        }
        return false;
    }

    return true;
}

void AndroidDevice::checkFilters(bool& filtersMatch, bool& filtersValid, const QStringList& filters, VerbosityEnum verbosityLevel, const QString& pid, const QString& tid, const QString& tag, const QString& text) const
{
    filtersMatch = verbosityLevel <= m_deviceWidget->getVerbosityLevel();

    if (!filtersMatch || !filtersValid)
    {
        return;
    }

    for (auto& filter : filters)
    {
        bool columnFound = false;
        if (!columnMatches("pid:", filter, pid, filtersValid, columnFound) ||
            !columnMatches("tid:", filter, tid, filtersValid, columnFound) ||
            !columnMatches("tag:", filter, tag, filtersValid, columnFound) ||
            !columnMatches("text:", filter, text, filtersValid, columnFound))
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

void AndroidDevice::reloadTextEdit()
{
    if (!m_didReadDeviceModel)
    {
        return;
    }

    qDebug() << "reloadTextEdit";
    m_deviceWidget->getTextEdit().clear();

    m_deviceLogFileStream->seek(0);
}

void AndroidDevice::maybeAddNewDevicesOfThisType(QPointer<QTabWidget> parent, DevicesMap& map, QPointer<DeviceAdapter> deviceAdapter)
{
    if (s_devicesListProcess.state() == QProcess::NotRunning)
    {
        for (auto& i : s_removedDeviceByTabClose)
        {
            i = false;  // not visited
        }

        if (s_devicesListProcess.canReadLine())
        {
            QString stringStream;
            QTextStream stream;
            stream.setCodec("UTF-8");
            stream.setString(&stringStream, QIODevice::ReadWrite | QIODevice::Text);
            stream << s_devicesListProcess.readAll();

            for (auto& dev : map)
            {
                dev->setVisited(false);
            }

            auto updateDeviceStatus = [](const QString& deviceStatus, BaseDevice& device, const QString& deviceId)
            {
                bool online = deviceStatus == "device";
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
                                        tr(getPlatformStringStatic()),
                                        tr("Initializing..."),
                                        deviceAdapter
                                    )
                                );
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
                if (!dev->isVisited())
                {
                    if (!s_removedDeviceByTabClose.contains(dev->getId()))
                    {
                        updateDeviceStatus("", *dev, dev->getId());
                    }
                }
            }

            for (auto it = s_removedDeviceByTabClose.begin(); it != s_removedDeviceByTabClose.end(); )
            {
                if (it.value() == false)  // became offline
                {
                    it = s_removedDeviceByTabClose.erase(it);
                }
                else
                {
                    ++it;
                }
            }

            s_devicesListProcess.close();
        }

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
