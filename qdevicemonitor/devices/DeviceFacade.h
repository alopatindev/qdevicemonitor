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

#ifndef DEVICEFACADE_H
#define DEVICEFACADE_H

#include "DataTypes.h"
#include "trackers/AndroidDevicesTracker.h"
#include "trackers/IOSDevicesTracker.h"
#include "trackers/TextFileDevicesTracker.h"
#include "trackers/usb/BaseUsbTracker.h"
#include "ui/colors/ColorTheme.h"

#include <QColor>
#include <QCompleter>
#include <QObject>
#include <QPointer>
#include <QStandardItemModel>
#include <QTabWidget>
#include <QTimer>
#include <QSettings>
#include <QStringList>
#include <QVector>

class DeviceWidget;

class DeviceFacade : public QObject
{
    Q_OBJECT

private:
    QPointer<QTabWidget> m_parent;

    AndroidDevicesTracker m_androidDevicesTracker;
    IOSDevicesTracker m_iOSDevicesTracker;
    TextFileDevicesTracker m_textFileDevicesTracker;
    const QVector<QPointer<BaseDevicesTracker>> m_trackers = {
        &m_androidDevicesTracker,
        &m_iOSDevicesTracker,
        &m_textFileDevicesTracker
    };

    QSharedPointer<BaseUsbTracker> m_usbTracker;

    DataTypes::DevicesMap m_devicesMap;

    QTimer m_filesRemovalTimer;
    QTimer m_trackersUpdateTimer;
    int m_trackersUpdateTries;

    int m_visibleBlocks;
    QString m_font;
    int m_fontSize;
    bool m_fontBold;
    bool m_darkTheme;
    QSharedPointer<ColorTheme> m_colorTheme;
    bool m_clearAndroidLog;
    int m_autoRemoveFilesHours;
    QStandardItemModel m_filterCompleterModel;
    QCompleter m_filterCompleter;
    QStringList m_filterCompletions;
    QString m_textEditorPath;

public:
    static const int LOG_REMOVAL_INTERVAL = 30 * 60 * 1000;
    static const int MAX_FILTER_COMPLETIONS = 60;

    static const int TRACKERS_UPDATE_INTERVAL = 1000;
    static const int MAX_TRACKERS_UPDATES_PER_USB_EVENT = 5;

    explicit DeviceFacade(QPointer<QTabWidget> parent = 0);
    ~DeviceFacade();

    void removeDeviceByTabIndex(const int index);
    void focusFilterInput();
    void markLog();
    void clearLog();
    void openLogFile();
    void writeToLogFile(const QString& id, const QString& line);

    void openTextFileDevice(const QString& fullPath);

    void loadSettings(const QSettings& s);
    void saveSettings(QSettings& s);
    void allDevicesReloadText();

    inline bool isDarkTheme() const { return m_darkTheme; }
    inline QColor getThemeColor(const ColorTheme::ColorType type) const { return m_colorTheme->getColor(type); }
    inline bool getClearAndroidLog() const { return m_clearAndroidLog; }
    inline const QString& getFont() const { return m_font; }
    inline int getFontSize() const { return m_fontSize; }
    inline bool isFontBold() const { return m_fontBold; }
    inline int getAutoRemoveFilesHours() const { return m_autoRemoveFilesHours; }
    inline int getVisibleLines() const { return m_visibleBlocks; }
    inline const QString& getTextEditorPath() const { return m_textEditorPath; }

    inline QCompleter& getFilterCompleter() { return m_filterCompleter; }
    void addFilterAsCompletion(const QString& completionToAdd);

    void emitUsbConnectionChange()
    {
        if (!m_usbTracker.isNull())
        {
            m_usbTracker->emitUsbConnectionChange();
        }
    }

private slots:
    void removeOldLogFiles();
    void onDeviceConnected(const DataTypes::DeviceType type, const QString& id);
    void onDeviceDisconnected(const DataTypes::DeviceType type, const QString& id);
    void trackersUpdate();
    void startTrackersUpdateTimer();

private:
    void fixTabIndexes(const int removedTabIndex);
    QPointer<DeviceWidget> getCurrentDeviceWidget();

    void initTrackersUpdater();
    void releaseTrackersUpdater();
};

#endif // DEVICEFACADE_H
