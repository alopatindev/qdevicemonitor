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
#include "ui/colors/ColorTheme.h"

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
)
    : BaseDevice(parent, id, type, getPlatformName(), humanReadableDescription, deviceFacade)
    , m_loggerStarted(false)
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

    if (m_tailProcess.state() == QProcess::NotRunning)
    {
        QStringList args;
        args.append("-F");
        args.append("-n");
        args.append(QString("%1").arg(m_deviceFacade->getVisibleLines()));
        args.append(m_id);
        m_tailProcess.start("tail", args);
    }

    m_loggerStarted = true;
}

void TextFileDevice::stopLogger()
{
    qDebug() << "TextFileDevice::stopLogger";

    m_loggerStarted = false;
    stopLogReadyTimer();

    if (m_tailProcess.state() != QProcess::NotRunning)
    {
        m_tailProcess.kill();
    }
}

void TextFileDevice::onUpdateFilter(const QString& filter)
{
    m_filters = filter.split(' ');
    m_filtersValid = true;
    reloadTextEdit();
    maybeAddCompletionAfterDelay(filter);
}

void TextFileDevice::checkFilters(bool& filtersMatch, bool& filtersValid, const QStringView text)
{
    filtersValid = true;

    const QString textString(text.toString());

    for (auto it = m_filters.constBegin(); it != m_filters.constEnd(); ++it)
    {
        const QStringView filter(*it);
        if (!columnTextMatches(filter, textString))
        {
            filtersMatch = false;
            break;
        }
    }
}

void TextFileDevice::filterAndAddToTextEdit(const QString& line)
{
    static const QRegularExpression re(
        "(?<prefix>[A-Za-z]{3} +[\\d]{1,2} [\\d:]{8}) (?<hostname>.+) ",
        QRegularExpression::InvertedGreedinessOption | QRegularExpression::DotMatchesEverythingOption
    );
    static const QString prefixPattern("prefix");
    static const QString hostnamePattern("hostname");

    bool filtersMatch = true;
    const QRegularExpressionMatch match = re.match(line);
    if (match.hasMatch())
    {
        const QStringView prefix = match.capturedView(QStringView(prefixPattern));
        const QStringView hostname = match.capturedView(QStringView(hostnamePattern));
        const QStringView text = QStringView(line).mid(match.capturedEnd("hostname") + 1);

        checkFilters(filtersMatch, m_filtersValid, text);

        if (filtersMatch)
        {
            m_deviceWidget->addText(ColorTheme::DateTime, prefix);
            m_deviceWidget->addText(ColorTheme::VerbosityWarn, hostname);
            m_deviceWidget->addText(ColorTheme::VerbosityVerbose, text);
            m_deviceWidget->flushText();
        }
    }
    else
    {
        checkFilters(filtersMatch, m_filtersValid, QStringView(line));
        if (filtersMatch)
        {
            m_deviceWidget->addText(ColorTheme::VerbosityVerbose, QStringView(line));
            m_deviceWidget->flushText();
        }
    }

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
    if (!m_loggerStarted)
    {
        return;
    }

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
        scheduleLogReady();
    }
}
