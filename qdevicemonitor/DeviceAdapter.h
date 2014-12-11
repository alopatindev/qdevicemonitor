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

    int m_visibleLines;
    QString m_font;
    int m_fontSize;
    bool m_darkTheme;
    int m_autoRemoveFilesHours;
    QStandardItemModel m_filterCompleterModel;
    QCompleter m_filterCompleter;
    QString m_completionToAdd;
    QTimer m_completionAddTimer;
    QStringList m_filterCompletions;

public:
    static const int UPDATE_FREQUENCY = 100;
    static const int MAX_FILTER_COMPLETIONS = 60;
    static const int COMPLETION_ADD_TIMEOUT = 10 * 1000;

    explicit DeviceAdapter(QPointer<QTabWidget> parent = 0);
    ~DeviceAdapter();

    void start();
    void stop();
    void loadSettings(const QSettings& s);
    void saveSettings(QSettings& s);

    bool isDarkTheme() const { return m_darkTheme; }
    inline const QString& getFont() const { return m_font; }
    int getFontSize() const { return m_fontSize; }
    int getAutoRemoveFilesHours() { return m_autoRemoveFilesHours; }

    inline QCompleter& getFilterCompleter() { return m_filterCompleter; }
    void maybeAddCompletionAfterDelay(const QString& filter);

signals:

public slots:
    void update();
    void addFilterAsCompletion();

private:
    void updateDevicesMap();
};

#endif // DEVICEADAPTER_H
