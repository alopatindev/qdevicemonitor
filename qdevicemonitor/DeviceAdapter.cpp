#include "DeviceAdapter.h"
#include "AndroidDevice.h"

using namespace DataTypes;

DeviceAdapter::DeviceAdapter(QPointer<QTabWidget> parent)
    : QObject(parent)
{
    update();
    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    m_updateTimer.start(1000);
}

void DeviceAdapter::update()
{
    updateDevicesMap();
}

void DeviceAdapter::updateDevicesMap()
{
    for (int t = 0; t != static_cast<int>(DeviceType::End); ++t)
    {
        DeviceType type = static_cast<DeviceType>(t);

        switch (type)
        {
        case DeviceType::TextFile:
            // TODO
            break;
        case DeviceType::Android:
            AndroidDevice::addNewDevicesOfThisType(static_cast<QTabWidget*>(parent()), m_devicesMap);
            break;
        case DeviceType::IOS:
            // TODO
            break;
        default:
            break;
        }
    }

    for (auto it = m_devicesMap.begin(); it != m_devicesMap.end(); ++it)
    {
        (*it)->update();
    }
}
