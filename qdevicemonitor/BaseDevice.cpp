#include "BaseDevice.h"

#include <QDebug>

using namespace DataTypes;

BaseDevice::BaseDevice(QPointer<QTabWidget> parent, const QString& id, DeviceType type,
                       const QString& humanReadableName, const QString& humanReadableDescription)
    : m_id(id)
    , m_type(type)
    , m_humanReadableName(humanReadableName)
    , m_humanReadableDescription(humanReadableDescription)
    , m_online(false)
    , m_tabWidget(parent)
    , m_tabIndex(-1)
{
    qDebug() << "new BaseDevice; type" << type << "; id" << id;
    setParent(parent);

    m_deviceWidget = new DeviceWidget(static_cast<QTabWidget*>(m_tabWidget));
    m_tabIndex = m_tabWidget->addTab(m_deviceWidget, humanReadableName);
}

void BaseDevice::updateTabWidget()
{
    m_tabWidget->setTabText(m_tabIndex, m_humanReadableName);
    m_tabWidget->setTabToolTip(m_tabIndex, m_humanReadableDescription);
    // TODO: m_tabWidget->setTabIcon, m_online
}

const QString& BaseDevice::getHumanReadableName() const
{
    return m_humanReadableName;
}

const QString& BaseDevice::getHumanReadableDescription() const
{
    return m_humanReadableDescription;
}

bool BaseDevice::isOnline() const
{
    return m_online;
}

void BaseDevice::setOnline(bool online)
{
    if (online != m_online)
    {
        qDebug() << m_id << "is now" << (online ? "online" : "offline");
        m_online = online;
        updateTabWidget();
    }
}
