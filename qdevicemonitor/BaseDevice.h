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

#ifndef BASEDEVICE_H
#define BASEDEVICE_H

#include "DeviceWidget.h"
#include "DeviceAdapter.h"
#include "DataTypes.h"

#include <QPointer>
#include <QProcess>
#include <QString>
#include <QTabWidget>
#include <QTimer>

using namespace DataTypes;

class BaseDevice : public QObject
{
    Q_OBJECT

public:
    explicit BaseDevice(QPointer<QTabWidget> parent, const QString& id, DeviceType type,
                        const QString& humanReadableName, const QString& humanReadableDescription,
                        QPointer<DeviceAdapter> deviceAdapter);
    virtual ~BaseDevice();

    void updateTabWidget();
    virtual void update() = 0;
    virtual void filterAndAddToTextEdit(const QString& line) = 0;
    virtual const char* getPlatformString() const = 0;

    void scheduleReloadTextEdit(int timeout = 500);
    void maybeAddCompletionAfterDelay();

    inline const QString& getHumanReadableName() const { return m_humanReadableName; }
    inline const QString& getHumanReadableDescription() const { return m_humanReadableDescription; }

    inline void setHumanReadableName(const QString& text) { m_humanReadableName = text; }
    inline void setHumanReadableDescription(const QString& text) { m_humanReadableDescription = text; }

    inline bool isOnline() const { return m_online; }
    void setOnline(bool online);

    inline void setVisited(bool visited) { m_visited = visited; }
    inline bool isVisited() const { return m_visited; }

    inline const QString& getId() const { return m_id; }

signals:

protected slots:
    virtual void reloadTextEdit() = 0;

protected:
    QString m_id;
    DeviceType m_type;
    QString m_humanReadableName;
    QString m_humanReadableDescription;
    bool m_online;
    QPointer<QTabWidget> m_tabWidget;
    QPointer<DeviceWidget> m_deviceWidget;
    int m_tabIndex;
    QPointer<DeviceAdapter> m_deviceAdapter;
    QString m_lastFilter;

private:
    QTimer m_reloadTextEditTimer;
    bool m_visited;
};

#endif // BASEDEVICE_H
