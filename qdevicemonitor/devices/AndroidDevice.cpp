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
#include "ui/colors/ColorTheme.h"

#include <QDebug>
#include <QHash>
#include <QRegularExpression>

using namespace DataTypes;

AndroidDevice::AndroidDevice(
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
    qDebug() << "AndroidDevice::AndroidDevice";
    m_deviceWidget->getFilterLineEdit().setToolTip(tr("Search for messages. Accepts<ul><li>Plain Text</li><li>Prefixes (<b>pid:</b>, <b>tid:</b>, <b>tag:</b> or <b>text:</b>) with Plain Text</li><li>Regular Expressions</li></ul>"));

    connect(&m_infoProcess, &QProcess::readyReadStandardOutput, this, &AndroidDevice::onUpdateModel);
    connect(&m_logProcess, &QProcess::readyReadStandardOutput, this, &BaseDevice::logReady);
    connect(&m_logProcess, &QProcess::stateChanged, this, &AndroidDevice::onLogProcessStatusChange);
    connect(m_deviceWidget.data(), &DeviceWidget::verbosityLevelChanged, this, &AndroidDevice::onVerbosityLevelChange);

    startInfoProcess();
}

AndroidDevice::~AndroidDevice()
{
    qDebug() << "AndroidDevice::~AndroidDevice";

    stopLogger();
    stopInfoProcess();

    disconnect(&m_infoProcess, &QProcess::readyReadStandardOutput, this, &AndroidDevice::onUpdateModel);
    disconnect(&m_logProcess, &QProcess::readyReadStandardOutput, this, &BaseDevice::logReady);
    disconnect(&m_logProcess, &QProcess::stateChanged, this, &AndroidDevice::onLogProcessStatusChange);
    disconnect(m_deviceWidget.data(), &DeviceWidget::verbosityLevelChanged, this, &AndroidDevice::onVerbosityLevelChange);
}

void AndroidDevice::stopInfoProcess()
{
    if (m_infoProcess.state() != QProcess::NotRunning)
    {
        qDebug() << "AndroidDevice::stopInfoProcess";
        m_infoProcess.kill();
    }
}

void AndroidDevice::startInfoProcess()
{
    qDebug() << "AndroidDevice::startInfoProcess" << m_id;
    if (m_infoProcess.state() == QProcess::NotRunning)
    {
        QStringList args;
        args.append("-s");
        args.append(m_id);
        args.append("shell");
        args.append("getprop");
        args.append("ro.product.model");
        m_infoProcess.setReadChannel(QProcess::StandardOutput);
        m_infoProcess.start("adb", args);
    }
}

void AndroidDevice::onUpdateModel()
{
    qDebug() << "AndroidDevice::onUpdateModel";

    const QString model = m_infoProcess.readLine().trimmed();
    if (!model.isEmpty())
    {
        qDebug() << "AndroidDevice::onUpdateModel" << m_id << "=>" << model;
        m_humanReadableName = model;
        updateTabWidget();
        m_didReadModel = true;
        startLogger();
    }

    stopInfoProcess();

    if (!m_didReadModel)
    {
        qDebug() << "AndroidDevice::onUpdateModel failed; retrying";
        startInfoProcess();
    }
}

void AndroidDevice::startLogger()
{
    qDebug() << "AndroidDevice::startLogger";

    if (!m_didReadModel)
    {
        qDebug() << "skipping; m_didReadModel =" << m_didReadModel;

        if (m_infoProcess.state() == QProcess::NotRunning)
        {
            startInfoProcess();
            qDebug() << "m_infoProcess was not running";
            return;
        }
        else if (m_logProcess.state() != QProcess::NotRunning)
        {
            qDebug() << "m_logProcess.state =" << m_logProcess.state();
            return;
        }
        else
        {
            qDebug() << "AndroidDevice::startLogger";
        }
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
        m_logFileStream->setCodec("UTF-8");

        startLogProcess();

        m_loggerStarted = true;
    }
}

void AndroidDevice::stopLogger()
{
    qDebug() << "AndroidDevice::stopLogger";

    m_loggerStarted = false;

    stopLogReadyTimer();
    stopLogProcess();
    m_logFileStream.clear();

    if (m_logFile.isOpen())
    {
        m_logFile.close();
    }
}

void AndroidDevice::startLogProcess()
{
    if (m_logProcess.state() == QProcess::NotRunning)
    {
        qDebug() << "AndroidDevice::startLogProcess";
        QStringList args;
        args.append("-s");
        args.append(m_id);
        args.append("logcat");
        args.append("-v");
        args.append("threadtime");
        args.append("*:v");
        m_logProcess.setReadChannel(QProcess::StandardOutput);
        m_logProcess.start("adb", args);
    }
}

void AndroidDevice::stopLogProcess()
{
    if (m_logProcess.state() != QProcess::NotRunning)
    {
        qDebug() << "AndroiDevice::stopLogProcess";
        m_logProcess.kill();
    }
}

void AndroidDevice::onUpdateFilter(const QString& filter)
{
    if (m_logProcess.state() == QProcess::Running)
    {
        m_filters = filter.split(' ');
        m_filtersValid = true;
        reloadTextEdit();
        maybeAddCompletionAfterDelay(filter);
    }
}

void AndroidDevice::onVerbosityLevelChange(const int level)
{
    (void) level;
    reloadTextEdit();
}

void AndroidDevice::onLogProcessStatusChange(const QProcess::ProcessState newState)
{
    qDebug() << "onLogProcessStatusChange" << newState;
    if (newState == QProcess::NotRunning)
    {
        setOnline(false);
    }
}

void AndroidDevice::filterAndAddToTextEdit(const QString& line)
{
    static const QRegularExpression re(
        "(?<date>[\\d-]+) *(?<time>[\\d:\\.]+) *(?<pid>\\d+) *(?<tid>\\d+) *(?<verbosity>[A-Z]) *(?<tag>.+):",
        QRegularExpression::InvertedGreedinessOption | QRegularExpression::DotMatchesEverythingOption
    );

    bool filtersMatch = true;
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

        const auto verbosityLevel = static_cast<VerbosityEnum>(Utils::verbosityCharacterToInt(verbosity.at(0).toLatin1()));

        checkFilters(filtersMatch, m_filtersValid, verbosityLevel, pid, tid, tag, text);

        if (filtersMatch)
        {
            const auto verbosityColorType = static_cast<ColorTheme::ColorType>(verbosityLevel);
            m_deviceWidget->addText(verbosityColorType, verbosity);
            m_deviceWidget->addText(ColorTheme::DateTime, date);
            m_deviceWidget->addText(ColorTheme::DateTime, time);
            m_deviceWidget->addText(ColorTheme::Pid, pid);
            m_deviceWidget->addText(ColorTheme::Tid, tid);
            m_deviceWidget->addText(ColorTheme::Tag, tag);
            m_deviceWidget->addText(verbosityColorType, text);
            m_deviceWidget->flushText();
        }
    }
    else
    {
        qDebug() << "failed to parse" << line;
        checkFilters(filtersMatch, m_filtersValid);
        if (filtersMatch)
        {
            m_deviceWidget->addText(ColorTheme::VerbosityVerbose, QStringRef(&line));
            m_deviceWidget->flushText();
        }
    }

    if (filtersMatch)
    {
        m_deviceWidget->maybeScrollTextEditToEnd();
    }

    m_deviceWidget->highlightFilterLineEdit(!m_filtersValid);
}

void AndroidDevice::checkFilters(bool& filtersMatch, bool& filtersValid, const VerbosityEnum verbosityLevel, const QStringRef& pid, const QStringRef& tid, const QStringRef& tag, const QStringRef& text)
{
    filtersMatch = verbosityLevel <= m_deviceWidget->getVerbosityLevel();

    if (!filtersMatch || !filtersValid)
    {
        return;
    }

    QString textString;
    bool textStringInitialized = false;

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

void AndroidDevice::reloadTextEdit()
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
    if (m_deviceFacade->getClearAndroidLog() && isOnline())
    {
        qDebug() << "clearing adb log";

        if (m_clearLogProcess.state() == QProcess::NotRunning)
        {
            QStringList args;
            args.append("-s");
            args.append(m_id);
            args.append("logcat");
            args.append("-c");
            m_clearLogProcess.setReadChannel(QProcess::StandardOutput);
            m_clearLogProcess.start("adb", args);
            m_clearLogProcess.waitForFinished(1000);
            m_clearLogProcess.kill();
        }
    }
}

void AndroidDevice::onLogReady()
{
    if (!m_loggerStarted)
    {
        return;
    }

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

    if (m_logProcess.canReadLine())
    {
        scheduleLogReady();
    }
}

void AndroidDevice::writeToLogFile(const QString& line)
{
    if (!m_logFileStream.isNull())
    {
        *m_logFileStream << line << "\n";
        m_logFileStream->flush();
    }
    BaseDevice::writeToLogFile(line);
}
