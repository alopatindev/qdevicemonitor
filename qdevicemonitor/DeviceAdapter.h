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

#ifndef DEVICEADAPTER_H
#define DEVICEADAPTER_H

#include "DataTypes.h"

#include <QCompleter>
#include <QObject>
#include <QPointer>
#include <QStandardItemModel>
#include <QTabWidget>
#include <QTimer>
#include <QSettings>
#include <QStringList>

class DeviceAdapter : public QObject
{
    Q_OBJECT

private:
    DataTypes::DevicesMap m_devicesMap;
    QTimer m_updateTimer;

    int m_visibleBlocks;
    QString m_font;
    int m_fontSize;
    bool m_fontBold;
    bool m_darkTheme;
    int m_autoRemoveFilesHours;
    QStandardItemModel m_filterCompleterModel;
    QCompleter m_filterCompleter;
    QStringList m_filterCompletions;

public:
    static const int UPDATE_FREQUENCY = 20;
    static const int MAX_FILTER_COMPLETIONS = 60;
    static const int COMPLETION_ADD_TIMEOUT = 10 * 1000;
    static const int MAX_LINES_UPDATE = 30;

    explicit DeviceAdapter(QPointer<QTabWidget> parent = 0);
    ~DeviceAdapter();

    void removeDeviceByTabIndex(int index);

    void start();
    void stop();
    void loadSettings(const QSettings& s);
    void saveSettings(QSettings& s);
    void allDevicesReloadText();

    inline bool isDarkTheme() const { return m_darkTheme; }
    inline const QString& getFont() const { return m_font; }
    inline int getFontSize() const { return m_fontSize; }
    inline bool isFontBold() const { return m_fontBold; }
    inline int getAutoRemoveFilesHours() const { return m_autoRemoveFilesHours; }
    inline int getVisibleBlocks() const { return m_visibleBlocks; }

    inline QCompleter& getFilterCompleter() { return m_filterCompleter; }
    void addFilterAsCompletion(const QString& completionToAdd);

signals:

public slots:
    void update();

private:
    void updateDevicesMap();
    void removeOldLogFiles();
};

#endif // DEVICEADAPTER_H
