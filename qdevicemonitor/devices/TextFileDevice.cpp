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
#include "ui/ThemeColors.h"

#include <QDebug>
#include <QFileInfo>
#include <QHash>
#include <QRegularExpression>

using namespace DataTypes;

TextFileDevice::TextFileDevice(
    QPointer<QTabWidget> parent,
    const QString& id,
    const DeviceType type,
    const QString& humanReadableDescription,
    QPointer<DeviceFacade> deviceFacade
) : BaseDevice(parent, id, type, getPlatformName(), humanReadableDescription, deviceFacade)
{
    qDebug() << "TextFileDevice::TextFileDevice";
    m_deviceWidget->getFilterLineEdit().setToolTip(tr("Search for messages. Accepts<ul><li>Plain Text</li><li>Prefix <b>text:</b> with Plain Text</li><li>Regular Expressions</li></ul>"));
    m_deviceWidget->hideVerbosity();
    m_deviceWidget->onLogFileNameChanged(id);

    connect(&m_tailProcess, &QProcess::readyReadStandardOutput, this, &BaseDevice::logReady);
    startLogger();
}

TextFileDevice::~TextFileDevice()
{
    qDebug() << "TextFileDevice::~TextFileDevice";
    disconnect(&m_tailProcess, &QProcess::readyReadStandardOutput, this, &BaseDevice::logReady);
    stopLogger();
}

void TextFileDevice::startLogger()
{
    qDebug() << "TextFileDevice::startLogger";

    QStringList args;
    args.append("-F");
    args.append("-n");
    args.append(QString("%1").arg(m_deviceFacade->getVisibleLines()));
    args.append(m_id);
    m_tailProcess.start("tail", args);
}

void TextFileDevice::stopLogger()
{
    qDebug() << "TextFileDevice::stopLogger";

    if (m_tailProcess.state() != QProcess::NotRunning)
    {
        m_tailProcess.terminate();
        m_tailProcess.kill();
        m_tailProcess.close();
    }
}

void TextFileDevice::onUpdateFilter(const QString& filter)
{
    if (m_tailProcess.state() == QProcess::Running)
    {
        m_filters = filter.split(' ');
        m_filtersValid = true;
        reloadTextEdit();
        maybeAddCompletionAfterDelay(filter);
    }
}

void TextFileDevice::checkFilters(bool& filtersMatch, bool& filtersValid, const QStringRef& text)
{
    filtersValid = true;

    const QString textString(text.toString());

    for (auto it = m_filters.constBegin(); it != m_filters.constEnd(); ++it)
    {
        const QStringRef filter(&(*it));
        if (!columnTextMatches(filter, textString))
        {
            filtersMatch = false;
            break;
        }
    }
}

void TextFileDevice::filterAndAddToTextEdit(const QString& line)
{
    bool filtersMatch = true;

    static const QRegularExpression re(
        "(?<prefix>[A-Za-z]* +[\\d]+ [\\d:]+) (?<hostname>.+) ",
        QRegularExpression::InvertedGreedinessOption | QRegularExpression::DotMatchesEverythingOption
    );

    const int themeIndex = m_deviceFacade->isDarkTheme() ? 1 : 0;
    const QRegularExpressionMatch match = re.match(line);
    if (match.hasMatch())
    {
        const QStringRef prefix = match.capturedRef("prefix");
        const QStringRef hostname = match.capturedRef("hostname");
        const QStringRef text = line.midRef(match.capturedEnd("hostname") + 1);

        checkFilters(filtersMatch, m_filtersValid, text);

        if (filtersMatch)
        {
            m_deviceWidget->addText(ThemeColors::Colors[themeIndex][ThemeColors::DateTime], prefix);
            m_deviceWidget->addText(ThemeColors::Colors[themeIndex][ThemeColors::VerbosityWarn], hostname);
            m_deviceWidget->addText(ThemeColors::Colors[themeIndex][ThemeColors::VerbosityVerbose], text);
            m_deviceWidget->flushText();
        }
    }
    else
    {
        checkFilters(filtersMatch, m_filtersValid, QStringRef(&line));
        if (filtersMatch)
        {
            m_deviceWidget->addText(ThemeColors::Colors[themeIndex][ThemeColors::VerbosityVerbose], QStringRef(&line));
            m_deviceWidget->flushText();
        }
    }

    m_deviceWidget->maybeScrollTextEditToEnd();
    //m_deviceWidget->highlightFilterLineEdit(!m_filtersValid);
}

void TextFileDevice::reloadTextEdit()
{
    qDebug() << "reloadTextEdit";
    m_deviceWidget->clearTextEdit();

    updateLogBufferSpace();
    filterAndAddFromLogBufferToTextEdit();
}

void TextFileDevice::onLogReady()
{
    QString line;
    for (int i = 0; i < MAX_LINES_UPDATE && m_tailProcess.canReadLine(); ++i)
    {
        m_tempStream << m_tailProcess.readLine();
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
        // FIXME: remove this hack
        line = m_tempStream.readLine();
#else
        m_tempStream.readLineInto(&line);
#endif
        addToLogBuffer(line);
        filterAndAddToTextEdit(line);
    }

    if (m_tailProcess.canReadLine())
    {
        emit logReady();
    }
}
