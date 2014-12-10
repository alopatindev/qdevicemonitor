#include "BaseDevice.h"

#include <QDebug>

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
{
    qDebug() << "new BaseDevice; type" << type << "; id" << id;

    m_deviceWidget = new DeviceWidget(static_cast<QTabWidget*>(m_tabWidget), m_deviceAdapter);
    m_deviceWidget->getFilterLineEdit().setCompleter(&m_deviceAdapter->getFilterCompleter());
    m_tabIndex = m_tabWidget->addTab(m_deviceWidget, humanReadableName);
    m_lastFilter = m_deviceWidget->getFilterLineEdit().text();

    m_reloadTextEditTimer.setSingleShot(true);
    connect(&m_reloadTextEditTimer, SIGNAL(timeout()), this, SLOT(reloadTextEdit()));
}

BaseDevice::~BaseDevice()
{
    disconnect(&m_reloadTextEditTimer, SIGNAL(timeout()));
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

void BaseDevice::setHumanReadableName(const QString& text)
{
    m_humanReadableName = text;
}

void BaseDevice::setHumanReadableDescription(const QString& text)
{
    m_humanReadableDescription = text;
}

void BaseDevice::scheduleReloadTextEdit(int timeout)
{
    m_reloadTextEditTimer.stop();
    m_reloadTextEditTimer.start(timeout);
}
