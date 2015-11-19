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

#include <QDebug>
#include <QIcon>

using namespace DataTypes;

BaseDevice::BaseDevice(QPointer<QTabWidget> parent, const QString& id, const DeviceType type,
                       const QString& humanReadableName, const QString& humanReadableDescription,
                       QPointer<DeviceAdapter> deviceAdapter)
    : QObject(parent)
    , m_id(id)
    , m_type(type)
    , m_humanReadableName(humanReadableName)
    , m_humanReadableDescription(humanReadableDescription)
    , m_online(false)
    , m_tabWidget(parent)
    , m_tabIndex(-1)
    , m_deviceAdapter(deviceAdapter)
    , m_filtersValid(true)
    , m_visited(true)
{
    qDebug() << "new BaseDevice; type" << type << "; id" << id;

    updateLogBufferSpace();

    m_deviceWidget = new DeviceWidget(static_cast<QTabWidget*>(m_tabWidget), m_deviceAdapter);
    m_deviceWidget->getFilterLineEdit().setCompleter(&m_deviceAdapter->getFilterCompleter());
    m_tabIndex = m_tabWidget->addTab(m_deviceWidget, humanReadableName);
    m_lastFilter = m_deviceWidget->getFilterLineEdit().text();

    m_completionAddTimer.setSingleShot(true);
    connect(&m_completionAddTimer, &QTimer::timeout, this, &BaseDevice::addFilterAsCompletion);
}

BaseDevice::~BaseDevice()
{
    qDebug() << "~BaseDevice" << m_id;

    disconnect(&m_completionAddTimer, 0, this, 0);
    disconnect(&m_deviceWidget->getFilterLineEdit(), 0, this, 0);

    delete m_deviceWidget; // FIXME: the design of QTabWidget is not really compatible with things like QSharedPointer at the moment
}

void BaseDevice::updateTabWidget()
{
    m_tabWidget->setTabText(m_tabIndex, m_humanReadableName);
    m_tabWidget->setTabToolTip(m_tabIndex, m_humanReadableDescription);

    const QIcon icon(QString(":/icons/%1_%2.png")
        .arg(getPlatformString())
        .arg(m_online ? "online" : "offline"));
    m_tabWidget->setTabIcon(m_tabIndex, icon);
}

void BaseDevice::setOnline(bool online)
{
    if (online != m_online)
    {
        qDebug() << m_id << "is now" << (online ? "online" : "offline");
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
    m_completionAddTimer.start(DeviceAdapter::COMPLETION_ADD_TIMEOUT);
}

void BaseDevice::addFilterAsCompletion()
{
    qDebug() << "BaseDevice::addFilterAsCompletion";
    m_deviceAdapter->addFilterAsCompletion(m_completionToAdd);
}

void BaseDevice::addToLogBuffer(const QString& text)
{
    if (m_logBuffer.size() >= m_deviceAdapter->getVisibleLines())
    {
        m_logBuffer.removeFirst();
    }
    m_logBuffer.append(text);
}

void BaseDevice::updateLogBufferSpace()
{
    qDebug() << "updateLogBufferSpace" << m_deviceAdapter->getVisibleLines();
    const int extraLines = m_logBuffer.size() - m_deviceAdapter->getVisibleLines();
    if (extraLines > 0)
    {
        qDebug() << "removing" << extraLines << "extra lines from log buffer";
        m_logBuffer.erase(m_logBuffer.begin(), m_logBuffer.begin() + extraLines);
    }
    m_logBuffer.reserve(m_deviceAdapter->getVisibleLines());
}

void BaseDevice::filterAndAddFromLogBufferToTextEdit()
{
    for (const QString& line : m_logBuffer)
    {
        filterAndAddToTextEdit(line);
    }
}
