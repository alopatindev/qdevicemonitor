#include "DeviceAdapter.h"
#include "AndroidDevice.h"
#include "SettingsDialog.h"

#include <QDebug>

using namespace DataTypes;

DeviceAdapter::DeviceAdapter(QPointer<QTabWidget> parent)
    : QObject(parent)
    , m_visibleLines(1000)
    , m_fontSize(12)
    , m_darkTheme(false)
    , m_autoRemoveFilesHours(48)
{
}

void DeviceAdapter::start()
{
    qDebug() << "DeviceAdapter::start";
    update();
    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    m_updateTimer.start(UPDATE_FREQUENCY);
}

void DeviceAdapter::stop()
{
    qDebug() << "DeviceAdapter::stop";
    m_updateTimer.stop();
    m_updateTimer.disconnect(SIGNAL(timeout()), this, SLOT(update()));
    AndroidDevice::stopDevicesListProcess();
}

void DeviceAdapter::update()
{
    updateDevicesMap();
}

void DeviceAdapter::updateDevicesMap()
{
    for (int t = 0; t != static_cast<int>(DeviceType::DeviceTypeEnd); ++t)
    {
        DeviceType type = static_cast<DeviceType>(t);

        switch (type)
        {
        case DeviceType::TextFile:
            // TODO
            break;
        case DeviceType::Android:
            AndroidDevice::addNewDevicesOfThisType(static_cast<QTabWidget*>(parent()), m_devicesMap, this);
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

void DeviceAdapter::loadSettings(const QSettings& s)
{
    QVariant visibleLines = s.value("visibleLines");
    if (visibleLines.isValid())
    {
        m_visibleLines = visibleLines.toInt();
    }

    QVariant font = s.value("font");
    if (font.isValid())
    {
        m_font = font.toString();
    }

    QVariant fontSize = s.value("fontSize");
    if (fontSize.isValid())
    {
        m_fontSize = fontSize.toInt();
    }

    QVariant darkTheme = s.value("darkTheme");
    if (darkTheme.isValid())
    {
        m_darkTheme = darkTheme.toBool();
    }

    QVariant autoRemoveFilesHours = s.value("autoRemoveFilesHours");
    if (autoRemoveFilesHours.isValid())
    {
        m_autoRemoveFilesHours = autoRemoveFilesHours.toInt();
    }
}

void DeviceAdapter::saveSettings(QSettings& s)
{
    s.setValue("visibleLines", m_visibleLines);
    s.setValue("font", m_font);
    s.setValue("fontSize", m_fontSize);
    s.setValue("darkTheme", m_darkTheme);
    s.setValue("autoRemoveFilesHours", m_autoRemoveFilesHours);
}
