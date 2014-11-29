#include "BaseDevice.h"
#include <QDebug>

using namespace DataTypes;

BaseDevice::BaseDevice(const QString& id, DeviceType type, const QString& humanReadableName, const QString& humanReadableDescription)
    : m_id(id)
    , m_type(type)
    , m_humanReadableName(humanReadableName)
    , m_humanReadableDescription(humanReadableDescription)
    , m_online(false)
{
    qDebug() << "new BaseDevice; type" << type << "; id" << id;
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
