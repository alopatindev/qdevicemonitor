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
#include <QRegExp>
#include <QStringList>
#include <QWeakPointer>

using namespace DataTypes;

static QProcess s_devicesListProcess;
static const char* PLATFORM_STRING = "Android";

AndroidDevice::AndroidDevice(QPointer<QTabWidget> parent, const QString& id, DeviceType type,
                             const QString& humanReadableName, const QString& humanReadableDescription, QPointer<DeviceAdapter> deviceAdapter)
    : BaseDevice(parent, id, type, humanReadableName, humanReadableDescription, deviceAdapter)
    , m_emptyTextEdit(true)
    , m_lastVerbosityLevel(m_deviceWidget->getVerbosityLevel())
    , m_didReadDeviceModel(false)
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
    m_deviceInfoProcess.start("adb", args);
    m_deviceInfoProcess.setReadChannel(QProcess::StandardOutput);
}

void AndroidDevice::startLogger()
{
    if (!m_didReadDeviceModel)
    {
        return;
    }

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
                startLogger();
            }
        }

        m_deviceInfoProcess.close();

        if (!m_didReadDeviceModel)
        {
            updateDeviceModel();
        }
    }

    if (m_deviceLogProcess.state() == QProcess::Running)
    {
        const QString filter = m_deviceWidget->getFilterLineEdit().text();
        if (m_deviceWidget->getVerbosityLevel() != m_lastVerbosityLevel)
        {
            m_lastVerbosityLevel = m_deviceWidget->getVerbosityLevel();
            scheduleReloadTextEdit();
        }
        else if (m_lastFilter.compare(filter) != 0)
        {
            m_lastFilter = filter;
            scheduleReloadTextEdit();
            m_deviceAdapter->maybeAddCompletionAfterDelay(filter);
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
    static QRegExp rx("([\\d-]+) *([\\d:\\.]+) *(\\d+) *(\\d+) *([A-Z]) *(.+):", Qt::CaseSensitive, QRegExp::W3CXmlSchema11);
    rx.setMinimal(true);

    QStringList filters = m_deviceWidget->getFilterLineEdit().text().split(" ");
    bool filtersMatch = true;
    bool filtersValid = true;

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
        checkFilters(filtersMatch, filtersValid, filters, verbosityLevel, pid, tid, tag, text);

        if (filtersMatch)
        {
            QColor verbosityColor = ThemeColors::Colors[theme][verbosityLevel];

            m_deviceWidget->addTextLine(verbosityColor, verbosity + " ");
            m_deviceWidget->addTextLine(ThemeColors::Colors[theme][ThemeColors::DateTime], date + " " + time + " ");
            m_deviceWidget->addTextLine(ThemeColors::Colors[theme][ThemeColors::Pid], pid + " ");
            m_deviceWidget->addTextLine(ThemeColors::Colors[theme][ThemeColors::Tid], tid + " ");
            m_deviceWidget->addTextLine(ThemeColors::Colors[theme][ThemeColors::Tag], tag + " ");
            m_deviceWidget->addTextLine(verbosityColor, text + "\n");
        }

        m_deviceWidget->maybeScrollTextEditToEnd();
    }
    else
    {
        qDebug() << "failed to parse" << line;
        checkFilters(filtersMatch, filtersValid, filters);
        if (filtersMatch)
        {
            m_deviceWidget->addTextLine(ThemeColors::Colors[theme][ThemeColors::VerbosityVerbose], line + "\n");
        }
    }

    m_deviceWidget->highlightFilterLineEdit(!filtersValid);
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
            QRegExp("", Qt::CaseSensitive, QRegExp::RegExp),
            QRegExp("", Qt::CaseSensitive, QRegExp::RegExp2),
            QRegExp("", Qt::CaseSensitive, QRegExp::Wildcard),
            QRegExp("", Qt::CaseSensitive, QRegExp::WildcardUnix),
            QRegExp("", Qt::CaseSensitive, QRegExp::FixedString),
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

    if (!filtersMatch)
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
