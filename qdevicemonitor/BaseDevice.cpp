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

BaseDevice::BaseDevice(QPointer<QTabWidget> parent, const QString& id, DeviceType type,
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
    , m_visited(true)
{
    qDebug() << "new BaseDevice; type" << type << "; id" << id;

    m_deviceWidget = new DeviceWidget(static_cast<QTabWidget*>(m_tabWidget), m_deviceAdapter);
    m_deviceWidget->getFilterLineEdit().setCompleter(&m_deviceAdapter->getFilterCompleter());
    m_tabIndex = m_tabWidget->addTab(m_deviceWidget, humanReadableName);
    m_lastFilter = m_deviceWidget->getFilterLineEdit().text();

    m_reloadTextEditTimer.setSingleShot(true);
    connect(&m_reloadTextEditTimer, SIGNAL(timeout()), this, SLOT(reloadTextEdit()));

    m_completionAddTimer.setSingleShot(true);
    connect(&m_completionAddTimer, SIGNAL(timeout()), this, SLOT(addFilterAsCompletion()));
}

BaseDevice::~BaseDevice()
{
    disconnect(&m_reloadTextEditTimer, SIGNAL(timeout()));
    disconnect(&m_completionAddTimer, SIGNAL(timeout()));

    delete m_deviceWidget; // FIXME: the design of QTabWidget is not really compatible with things like QSharedPointer at the moment
}

void BaseDevice::updateTabWidget()
{
    m_tabWidget->setTabText(m_tabIndex, m_humanReadableName);
    m_tabWidget->setTabToolTip(m_tabIndex, m_humanReadableDescription);

    QIcon icon(QString(":/icons/%1_%2.png")
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
    }
}

void BaseDevice::scheduleReloadTextEdit(int timeout)
{
    m_reloadTextEditTimer.stop();
    m_reloadTextEditTimer.start(timeout);
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
