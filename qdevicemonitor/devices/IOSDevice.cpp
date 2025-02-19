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
#include "ui/colors/ColorTheme.h"

#include <QDebug>
#include <QFileInfo>
#include <QRegularExpression>

using namespace DataTypes;

IOSDevice::IOSDevice(
    QPointer<QTabWidget> parent,
    const QString& id,
    const DeviceType type,
    const QString& humanReadableDescription,
    QPointer<DeviceFacade> deviceFacade
)
    : BaseDevice(parent, id, type, getPlatformName(), humanReadableDescription, deviceFacade)
    , m_didReadModel(false)
    , m_loggerStarted(false)
{
    qDebug() << "IOSDevice::IOSDevice";

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    m_tempErrorsStream.setCodec("UTF-8");
#else
    m_tempErrorsStream.setEncoding(QStringConverter::Utf8);
#endif
    m_tempErrorsStream.setString(&m_tempErrorsBuffer, QIODevice::ReadWrite | QIODevice::Text);

    m_deviceWidget->getFilterLineEdit().setToolTip(tr("Search for messages. Accepts<ul><li>Plain Text</li><li>Prefix <b>text:</b> with Plain Text</li><li>Regular Expressions</li></ul>"));
    m_deviceWidget->hideVerbosity();

    connect(&m_logProcess, &QProcess::readyReadStandardOutput, this, &BaseDevice::logReady);
    connect(&m_infoProcess, &QProcess::readyReadStandardError, this, &IOSDevice::onErrorsReady);
    connect(&m_infoProcess, &QProcess::readyReadStandardOutput, this, &IOSDevice::onUpdateModel);

    startInfoProcess();
}

IOSDevice::~IOSDevice()
{
    qDebug() << "IOSDevice::~IOSDevice";

    stopLogger();
    stopInfoProcess();

    disconnect(&m_logProcess, &QProcess::readyReadStandardOutput, this, &BaseDevice::logReady);
    disconnect(&m_infoProcess, &QProcess::readyReadStandardError, this, &IOSDevice::onErrorsReady);
    disconnect(&m_infoProcess, &QProcess::readyReadStandardOutput, this, &IOSDevice::onUpdateModel);
}

void IOSDevice::stopInfoProcess()
{
    if (m_infoProcess.state() != QProcess::NotRunning)
    {
        m_infoProcess.kill();
    }
}

void IOSDevice::startInfoProcess()
{
    qDebug() << "IOSDevice::startInfoProcess" << m_id;
    if (m_infoProcess.state() == QProcess::NotRunning)
    {
        QStringList args;
        args.append("-u");
        args.append(m_id);
        args.append("-s");
        args.append("-k");
        args.append("DeviceName");
        m_infoProcess.setReadChannel(QProcess::StandardOutput);
        m_infoProcess.start("ideviceinfo", args);
    }
}

void IOSDevice::onUpdateModel()
{
    qDebug() << "IOSDevice::onUpdateModel";

    if (m_infoProcess.canReadLine())
    {
        const QString model = m_infoProcess.readLine().trimmed();
        if (!model.isEmpty())
        {
            qDebug() << "IOSDevice::onUpdateModel" << m_id << "=>" << model;
            m_humanReadableName = model;
            updateTabWidget();
            m_didReadModel = true;
            startLogger();
        }
    }

    stopInfoProcess();

    if (!m_didReadModel)
    {
        qDebug() << "IOSDevice::onUpdateModel failed; retrying";
        startInfoProcess();
    }
}

void IOSDevice::startLogProcess()
{
    if (m_logProcess.state() == QProcess::NotRunning)
    {
        qDebug() << "IOSDevice::startLogProcess";
        QStringList args;
        args.append("-u");
        args.append(m_id);
        m_logProcess.setReadChannel(QProcess::StandardOutput);
        m_logProcess.start("idevicesyslog", args);
    }
}

void IOSDevice::stopLogProcess()
{
    if (m_logProcess.state() != QProcess::NotRunning)
    {
        qDebug() << "IOSDevice::stopLogProcess";
        m_logProcess.kill();
    }
}

void IOSDevice::startLogger()
{
    if (!m_didReadModel || m_logProcess.state() != QProcess::NotRunning)
    {
        qDebug() << "IOSDevice::startLogger skipping; m_didReadModel =" << m_didReadModel << "m_logProcess.state =" << m_logProcess.state();
        return;
    }
    else
    {
        qDebug() << "IOSDevice::startLogger";
    }

    if (!m_loggerStarted)
    {
        const QString currentLogAbsFileName = Utils::getNewLogFilePath(
            QString("%1-%2-")
                .arg(getPlatformName())
                .arg(Utils::removeSpecialCharacters(m_humanReadableName))
        );
        m_currentLogFileName = currentLogAbsFileName;
        m_deviceWidget->onLogFileNameChanged(m_currentLogFileName);

        m_logFile.setFileName(currentLogAbsFileName);
        m_logFile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
        m_logFileStream = QSharedPointer<QTextStream>::create(&m_logFile);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        m_logFileStream->setCodec("UTF-8");
#else
        m_logFileStream->setEncoding(QStringConverter::Utf8);
#endif

        startLogProcess();

        m_loggerStarted = true;
    }
}

void IOSDevice::stopLogger()
{
    qDebug() << "IOSDevice::stopLogger";

    m_loggerStarted = false;

    stopLogReadyTimer();
    stopLogProcess();

    m_logFileStream.clear();

    if (m_logFile.isOpen())
    {
        m_logFile.close();
    }
}

void IOSDevice::onUpdateFilter(const QString& filter)
{
    m_filters = filter.split(' ');
    m_filtersValid = true;
    reloadTextEdit();
    maybeAddCompletionAfterDelay(filter);
}

void IOSDevice::checkFilters(bool& filtersMatch, bool& filtersValid, const QStringView text)
{
    QString textString;
    bool textStringInitialized = false;

    for (auto it = m_filters.constBegin(); it != m_filters.constEnd(); ++it)
    {
        const QStringView filter(*it);
        bool columnFound = false;
        if (!columnMatches("text:", filter, text, filtersValid, columnFound))
        {
            filtersMatch = false;
            break;
        }

        if (!columnFound)
        {
            if (!textStringInitialized)
            {
                textStringInitialized = true;
                textString = text.toString();
            }

            if (!columnTextMatches(filter, textString))
            {
                filtersMatch = false;
                break;
            }
        }
    }
}

void IOSDevice::filterAndAddToTextEdit(const QString& line)
{
    if (line == QString("[connected]") || line == QString("[disconnected]"))
    {
        m_deviceFacade->emitUsbConnectionChange();
        return;
    }

    static const QRegularExpression re(
        "(?<prefix>[A-Za-z]* +[\\d]+ [\\d:]+) (?<deviceName>.+) ",
        QRegularExpression::InvertedGreedinessOption | QRegularExpression::DotMatchesEverythingOption
    );

    bool filtersMatch = true;
    const QRegularExpressionMatch match = re.match(line);
    if (match.hasMatch())
    {
        const QStringView prefix = match.captured("prefix");
        const QStringView deviceName = match.captured("deviceName");
        const QStringView text = QStringView(line).mid(match.capturedEnd("deviceName") + 1);

        checkFilters(filtersMatch, m_filtersValid, text);

        if (filtersMatch)
        {
            m_deviceWidget->addText(ColorTheme::DateTime, prefix);
            m_deviceWidget->addText(ColorTheme::VerbosityWarn, deviceName);
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

    m_deviceWidget->highlightFilterLineEdit(!m_filtersValid);
}

void IOSDevice::reloadTextEdit()
{
    if (!m_didReadModel)
    {
        return;
    }

    qDebug() << "reloadTextEdit";
    m_deviceWidget->clearTextEdit();

    updateLogBufferSpace();
    filterAndAddFromLogBufferToTextEdit();
}

void IOSDevice::onLogReady()
{
    if (!m_loggerStarted)
    {
        return;
    }

    maybeReadErrorsPart();
    const bool allErrorsAreRead = m_tempErrorsStream.atEnd();

    if (allErrorsAreRead)
    {
        maybeReadLogPart();
    }

    if (m_logProcess.canReadLine() || !allErrorsAreRead)
    {
        scheduleLogReady();
    }
}

void IOSDevice::onErrorsReady()
{
    m_tempErrorsStream << m_infoProcess.readAllStandardError();
}

void IOSDevice::maybeReadErrorsPart()
{
    QString line;
    for (int i = 0; i < MAX_LINES_UPDATE && !m_tempErrorsStream.atEnd(); ++i)
    {
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
        // FIXME: remove this hack
        line = m_tempErrorsStream.readLine();
        if (!line.isEmpty())
#else
        if (m_tempErrorsStream.readLineInto(&line))
#endif
        {
            m_deviceWidget->addText(ColorTheme::VerbosityAssert, QStringView(line));
            m_deviceWidget->flushText();
        }
    }
}

void IOSDevice::maybeReadLogPart()
{
    QString line;
    for (int i = 0; i < MAX_LINES_UPDATE && m_logProcess.canReadLine(); ++i)
    {
        m_tempStream << m_logProcess.readLine();
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
        // FIXME: remove this hack
        line = m_tempStream.readLine();
        if (!line.isEmpty())
#else
        if (m_tempStream.readLineInto(&line))
#endif
        {
            writeToLogFile(line);
            filterAndAddToTextEdit(line);
        }
    }
}

void IOSDevice::writeToLogFile(const QString& line)
{
    if (!m_logFileStream.isNull())
    {
        *m_logFileStream << line << "\n";
        m_logFileStream->flush();
    }
    BaseDevice::writeToLogFile(line);
}
