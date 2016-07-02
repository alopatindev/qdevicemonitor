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

#include "BaseDevice.h"

#include "AndroidDevice.h"
#include "IOSDevice.h"
#include "TextFileDevice.h"

#include <QDebug>
#include <QIcon>
#include <QtCore/QStringBuilder>

using namespace DataTypes;

BaseDevice::BaseDevice(
    QPointer<QTabWidget> parent,
    const QString& id, const DeviceType type,
    const QString& humanReadableName,
    const QString& humanReadableDescription,
    QPointer<DeviceFacade> deviceFacade
)
    : QObject(parent)
    , m_id(id)
    , m_type(type)
    , m_humanReadableName(humanReadableName)
    , m_humanReadableDescription(humanReadableDescription)
    , m_online(false)
    , m_tabWidget(parent)
    , m_tabIndex(-1)
    , m_deviceFacade(deviceFacade)
    , m_filtersValid(true)
{
    qDebug() << "new BaseDevice; type" << type << "; id" << id;

    m_tempStream.setCodec("UTF-8");
    m_tempStream.setString(&m_tempBuffer, QIODevice::ReadWrite | QIODevice::Text);

    updateLogBufferSpace();

    m_deviceWidget = QSharedPointer<DeviceWidget>::create(static_cast<QTabWidget*>(m_tabWidget), m_deviceFacade, id);
    m_deviceWidget->getFilterLineEdit().setCompleter(&m_deviceFacade->getFilterCompleter());
    m_tabIndex = m_tabWidget->addTab(m_deviceWidget.data(), humanReadableName);

    m_completionAddTimer.setSingleShot(true);
    m_logReadyTimer.setSingleShot(true);

    connect(&m_logReadyTimer, &QTimer::timeout, this, &BaseDevice::onLogReady);
    connect(&m_completionAddTimer, &QTimer::timeout, this, &BaseDevice::addFilterAsCompletion);
    connect(&(m_deviceWidget->getFilterLineEdit()), &QLineEdit::textChanged, this, &BaseDevice::updateFilter);
    connect(this, &BaseDevice::logReady, this, &BaseDevice::onLogReady);
}

BaseDevice::~BaseDevice()
{
    qDebug() << "~BaseDevice" << m_id;

    disconnect(&m_logReadyTimer, nullptr, this, nullptr);
    disconnect(&m_completionAddTimer, nullptr, this, nullptr);
    disconnect(&m_deviceWidget->getFilterLineEdit(), nullptr, this, nullptr);
    disconnect(this, &BaseDevice::logReady, this, &BaseDevice::onLogReady);

    m_tabWidget.clear();
}

QSharedPointer<BaseDevice> BaseDevice::create(
    QPointer<QTabWidget> parent,
    QPointer<DeviceFacade> deviceFacade,
    const DeviceType type,
    const QString& id
)
{
    const QString description = tr("Initializing...");
    switch (type)
    {
    case DeviceType::TextFile:
        return QSharedPointer<TextFileDevice>::create(
            parent, id, type, description, deviceFacade
        );
    case DeviceType::Android:
        return QSharedPointer<AndroidDevice>::create(
            parent, id, type, description, deviceFacade
        );
    case DeviceType::IOS:
        return QSharedPointer<IOSDevice>::create(
            parent, id, type, description, deviceFacade
        );
    default:
        Q_ASSERT_X(false, "BaseDevice::create", "device is not implemented");
        return QSharedPointer<BaseDevice>();
    }
}

void BaseDevice::updateTabWidget()
{
    m_tabWidget->setTabText(m_tabIndex, m_humanReadableName);
    m_tabWidget->setTabToolTip(m_tabIndex, m_humanReadableDescription);

    const QIcon icon(QString(":/icons/%1_%2.png")
        .arg(getPlatformName())
        .arg(m_online ? "online" : "offline"));
    m_tabWidget->setTabIcon(m_tabIndex, icon);
}

void BaseDevice::setOnline(const bool online)
{
    if (online != m_online)
    {
        qDebug() << m_id << "is now" << (online ? "online" : "offline");

        updateInfo(online);

        m_online = online;
        updateTabWidget();

        if (online)
        {
            reloadTextEdit();
        }

        onOnlineChange(online);
    }
}

void BaseDevice::maybeAddCompletionAfterDelay(const QString& filter)
{
    qDebug() << "BaseDevice::maybeAddCompletionAfterDelay" << filter;
    m_completionToAdd = filter;
    m_completionAddTimer.stop();
    m_completionAddTimer.start(COMPLETION_ADD_TIMEOUT);
}

void BaseDevice::addFilterAsCompletion()
{
    qDebug() << "BaseDevice::addFilterAsCompletion";
    m_deviceFacade->addFilterAsCompletion(m_completionToAdd);
}

void BaseDevice::scheduleLogReady()
{
    m_logReadyTimer.start(LOG_READY_TIMEOUT);
}

void BaseDevice::stopLogReadyTimer()
{
    qDebug() << "BaseDevice::stopLogReadyTimer";
    m_logReadyTimer.stop();
}

void BaseDevice::updateFilter(const QString& filter)
{
    qDebug() << "BaseDevice::updateFilter(" << filter << ")";

    const QString regexpFilter(".*(" % filter % ").*");
    m_columnTextRegexp.setPattern(regexpFilter);
    m_columnTextRegexp.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);

    onUpdateFilter(filter);
}

void BaseDevice::addToLogBuffer(const QString& text)
{
    m_logBuffer->push(text);
}

void BaseDevice::updateLogBufferSpace()
{
    const size_t lines = static_cast<size_t>(m_deviceFacade->getVisibleLines());
    if (m_logBuffer.isNull() || m_logBuffer->getCapacity() != lines)
    {
        qDebug() << "updateLogBufferSpace" << lines;
        m_logBuffer = QSharedPointer<StringRingBuffer>::create(m_deviceFacade->getVisibleLines());
    }
}

void BaseDevice::filterAndAddFromLogBufferToTextEdit()
{
    for (auto it = m_logBuffer->constBegin(); it.isValid(); it++)
    {
        filterAndAddToTextEdit(*it);
    }
}

bool BaseDevice::columnMatches(const QString& column, const QStringRef& filter, const QStringRef& originalValue, bool& filtersValid, bool& columnFound)
{
    if (filter.startsWith(column))
    {
        columnFound = true;
        const QStringRef value = filter.mid(column.length());
        if (value.isEmpty())
        {
            filtersValid = false;
        }
        else if (!originalValue.contains(value))
        {
            return false;
        }
    }
    return true;
}

bool BaseDevice::columnTextMatches(const QStringRef& filter, const QString& text)
{
    if (filter.isEmpty() || text.contains(filter))
    {
        return true;
    }
    else
    {
        const QRegularExpressionMatch match = m_columnTextRegexp.match(text);
        return match.hasMatch();
    }

    return true;
}

void BaseDevice::updateInfo(const bool online, const QString& additional)
{
    const QString additionalWithNewLine = additional.isEmpty() ? additional : ("\n" % additional);
    setHumanReadableDescription(
        tr("%1\nStatus: %2\nID: %3%4")
            .arg(tr(getPlatformName()))
            .arg(tr(online ? "Online" : "Offline"))
            .arg(m_id)
            .arg(additionalWithNewLine)
    );
}
