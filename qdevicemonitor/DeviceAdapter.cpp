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
    m_filterCompleter.setModel(&m_filterCompleterModel);
    m_completionAddTimer.setSingleShot(true);
    connect(&m_completionAddTimer, SIGNAL(timeout()), this, SLOT(addFilterAsCompletion()));
}

DeviceAdapter::~DeviceAdapter()
{
    disconnect(&m_completionAddTimer, SIGNAL(timeout()));
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

    for (auto& device : m_devicesMap)
    {
        device->update();
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
    else
    {
        m_font = "monospace";
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

    QVariant filterCompletions = s.value("filterCompletions");
    if (filterCompletions.isValid())
    {
        m_filterCompletions = filterCompletions.toStringList();
        m_filterCompleterModel.clear();
        for (const QString& filter : m_filterCompletions)
        {
            m_filterCompleterModel.appendRow(new QStandardItem(filter));
        }
    }
}

void DeviceAdapter::saveSettings(QSettings& s)
{
    qDebug() << "DeviceAdapter::saveSettings";
    s.setValue("visibleLines", m_visibleLines);
    s.setValue("font", m_font);
    s.setValue("fontSize", m_fontSize);
    s.setValue("darkTheme", m_darkTheme);
    s.setValue("autoRemoveFilesHours", m_autoRemoveFilesHours);
    s.setValue("filterCompletions", m_filterCompletions);
}

void DeviceAdapter::maybeAddCompletionAfterDelay(const QString& filter)
{
    qDebug() << "DeviceAdapter::maybeAddCompletionAfterDelay" << filter;
    m_completionToAdd = filter;
    m_completionAddTimer.stop();
    m_completionAddTimer.start(COMPLETION_ADD_TIMEOUT);
}

void DeviceAdapter::addFilterAsCompletion()
{
    qDebug() << "addFilterAsCompletion" << m_completionToAdd;
    if (m_filterCompletions.indexOf(m_completionToAdd) != -1)
    {
        qDebug() << m_completionToAdd << "is already in the list";
        return;
    }

    m_filterCompleterModel.appendRow(new QStandardItem(m_completionToAdd));
    m_filterCompletions.append(m_completionToAdd);

    size_t oldCompletionsNumber = m_filterCompletions.size() > MAX_FILTER_COMPLETIONS
                                  ? m_filterCompletions.size() - MAX_FILTER_COMPLETIONS
                                  : 0;
    if (oldCompletionsNumber > 0)
    {
        qDebug() << "removing old" << oldCompletionsNumber << "completions";
        m_filterCompletions.erase(m_filterCompletions.begin(), m_filterCompletions.begin() + oldCompletionsNumber);
        m_filterCompleterModel.removeRows(0, int(oldCompletionsNumber));
    }
}
