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

#include "TextFileDevice.h"
#include "Utils.h"
#include "ThemeColors.h"

#include <QDebug>
#include <QFileInfo>
#include <QHash>
#include <QRegExp>

using namespace DataTypes;

static QStringList s_filesToOpen;

TextFileDevice::TextFileDevice(QPointer<QTabWidget> parent, const QString& id, DeviceType type,
                               const QString& humanReadableName, const QString& humanReadableDescription, QPointer<DeviceAdapter> deviceAdapter)
    : BaseDevice(parent, id, type, humanReadableName, humanReadableDescription, deviceAdapter)
{
    qDebug() << "TextFileDevice::TextFileDevice";
    m_deviceWidget->hideVerbosity();
    m_deviceWidget->onLogFileNameChanged(id);

    startLogger();
}

TextFileDevice::~TextFileDevice()
{
    qDebug() << "TextFileDevice::~TextFileDevice";
    stopLogger();
}

void TextFileDevice::startLogger()
{
    qDebug() << "TextFileDevice::startLogger";

    QStringList args;
    args.append("-F");
    args.append("-n");
    const int n = m_filters.count() > 0 ? 3 : 1;  // FIXME
    args.append(QString("%1").arg(m_deviceAdapter->getVisibleBlocks() * n));
    args.append(m_id);
    m_tailProcess.start("tail", args);
}

void TextFileDevice::stopLogger()
{
    qDebug() << "TextFileDevice::stopLogger";

    m_tailProcess.close();
}

void TextFileDevice::update()
{
    switch (m_tailProcess.state())
    {
    case QProcess::Running:
        {
            const QString filter = m_deviceWidget->getFilterLineEdit().text();
            if (m_lastFilter.compare(filter) != 0)
            {
                m_filters = filter.split(" ");
                m_filtersValid = true;
                m_lastFilter = filter;
                scheduleReloadTextEdit();
                maybeAddCompletionAfterDelay(filter);
            }

            QString stringStream;
            QTextStream stream;
            stream.setCodec("UTF-8");
            stream.setString(&stringStream, QIODevice::ReadWrite | QIODevice::Text);

            for (int i = 0; i < DeviceAdapter::MAX_LINES_UPDATE && m_tailProcess.canReadLine(); ++i)
            {
                stream << m_tailProcess.readLine();
                filterAndAddToTextEdit(stream.readLine());
            }
        }
    case QProcess::NotRunning:
    case QProcess::Starting:
    default:
        break;
    }
}

void TextFileDevice::checkFilters(bool& filtersMatch, bool& filtersValid, const QStringList& filters, const QString& text) const
{
    for (auto& filter : filters)
    {
        bool columnFound = false;
        if (!Utils::columnMatches("text:", filter, text, filtersValid, columnFound))
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

void TextFileDevice::filterAndAddToTextEdit(const QString& line)
{
    bool filtersMatch = true;

    static QRegExp rx("([A-Za-z]* +[\\d]+ [\\d:]+) (.+) ", Qt::CaseSensitive, QRegExp::RegExp2);
    rx.setMinimal(true);


    int theme = m_deviceAdapter->isDarkTheme() ? 1 : 0;
    if (rx.indexIn(line) > -1)
    {
        const QString prefix = rx.cap(1);
        const QString hostname = rx.cap(2);
        const QString text = line.mid(rx.pos(2) + rx.cap(2).length() + 1);

        /*qDebug() << "prefix" << prefix;
        qDebug() << "hostname" << hostname;
        qDebug() << "text" << text;*/

        bool filtersMatch = true;
        checkFilters(filtersMatch, m_filtersValid, m_filters, text);

        if (filtersMatch)
        {
            m_deviceWidget->addText(ThemeColors::Colors[theme][ThemeColors::DateTime], prefix + " ");
            m_deviceWidget->addText(ThemeColors::Colors[theme][ThemeColors::VerbosityWarn], hostname + " ");
            m_deviceWidget->addText(ThemeColors::Colors[theme][ThemeColors::VerbosityVerbose], text + "\n");
        }
    }
    else
    {
        checkFilters(filtersMatch, m_filtersValid, m_filters, line);
        if (filtersMatch)
        {
            m_deviceWidget->addText(ThemeColors::Colors[theme][ThemeColors::VerbosityVerbose], line + "\n");
        }
    }

    m_deviceWidget->maybeScrollTextEditToEnd();
    //m_deviceWidget->highlightFilterLineEdit(!m_filtersValid);
}

void TextFileDevice::reloadTextEdit()
{
    qDebug() << "reloadTextEdit";
    m_deviceWidget->clearTextEdit();
    stopLogger();
    startLogger();
}

void TextFileDevice::maybeAddNewDevicesOfThisType(QPointer<QTabWidget> parent, DevicesMap& map, QPointer<DeviceAdapter> deviceAdapter)
{
    for (const auto& logFile : s_filesToOpen)
    {
        auto it = map.find(logFile);
        if (it == map.end())
        {
            QString fileName = QFileInfo(logFile).fileName();
            map[logFile] = QSharedPointer<BaseDevice>(
                new TextFileDevice(
                    parent,
                    logFile,
                    DeviceType::TextFile,
                    fileName,
                    logFile,
                    deviceAdapter
                )
            );
        }
    }

    s_filesToOpen.clear();
}

void TextFileDevice::openLogFile(const QString& logFile)
{
    s_filesToOpen.append(logFile);
}
