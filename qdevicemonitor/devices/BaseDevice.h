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

#include "ui/DeviceWidget.h"
#include "DeviceFacade.h"
#include "DataTypes.h"
#include "StringRingBuffer.h"

#include <QPointer>
#include <QProcess>
#include <QRegularExpression>
#include <QSharedPointer>
#include <QString>
#include <QTabWidget>
#include <QTextStream>
#include <QTimer>

using namespace DataTypes;

class BaseDevice : public QObject
{
    Q_OBJECT

public:
    static const int MAX_LINES_UPDATE = 30;
    static const int COMPLETION_ADD_TIMEOUT = 10 * 1000;
    static const int LOG_READY_TIMEOUT = 1;

    static QSharedPointer<BaseDevice> create(
        QPointer<QTabWidget> parent,
        QPointer<DeviceFacade> deviceFacade,
        const DeviceType type,
        const QString& id
    );

    explicit BaseDevice(
        QPointer<QTabWidget> parent,
        const QString& id,
        const DeviceType type,
        const QString& humanReadableName,
        const QString& humanReadableDescription,
        QPointer<DeviceFacade> deviceFacade
    );
    ~BaseDevice() override;

    void updateTabWidget();
    virtual void onUpdateFilter(const QString& filter) = 0;
    virtual void filterAndAddToTextEdit(const QString& line) = 0;
    virtual const char* getPlatformName() const = 0;
    virtual void reloadTextEdit() = 0;

    void maybeAddCompletionAfterDelay(const QString& filter);

    inline const QString& getHumanReadableName() const { return m_humanReadableName; }
    inline const QString& getHumanReadableDescription() const { return m_humanReadableDescription; }

    inline void setHumanReadableName(const QString& text) { m_humanReadableName = text; }
    inline void setHumanReadableDescription(const QString& text) { m_humanReadableDescription = text; }

    inline bool isOnline() const { return m_online; }
    void setOnline(const bool online);
    virtual void onOnlineChange(const bool online) { (void) online; }

    inline int getTabIndex() const { return m_tabIndex; }
    inline void setTabIndex(const int tabIndex) { m_tabIndex = tabIndex; }

    inline const QString& getId() const { return m_id; }
    inline DeviceType getType() const { return m_type; }
    void updateInfo(const bool online, const QString& additional = QString());

    inline const QString& getCurrentLogFileName() const { return m_currentLogFileName; }

    void addToLogBuffer(const QString& text);
    virtual void writeToLogFile(const QString& line) { addToLogBuffer(line); }

    void updateLogBufferSpace();
    void filterAndAddFromLogBufferToTextEdit();
    bool columnMatches(const QString& column, const QStringView filter, const QStringView originalValue, bool& filtersValid, bool& columnFound);
    bool columnTextMatches(const QStringView filter, const QString& text);

    void scheduleLogReady();
    void stopLogReadyTimer();

signals:
    void logReady();

private slots:
    void addFilterAsCompletion();
    void updateFilter(const QString& filter);
    virtual void onLogReady() = 0;

protected:
    const QString m_id;
    DeviceType m_type;
    QString m_humanReadableName;
    QString m_humanReadableDescription;
    bool m_online;
    QPointer<QTabWidget> m_tabWidget;
    QSharedPointer<DeviceWidget> m_deviceWidget;
    int m_tabIndex;
    QPointer<DeviceFacade> m_deviceFacade;
    QString m_currentLogFileName;
    bool m_dirtyFilter;
    bool m_filtersValid;
    QStringList m_filters;
    QSharedPointer<StringRingBuffer> m_logBuffer;
    QRegularExpression m_columnTextRegexp;
    QString m_tempBuffer;
    QTextStream m_tempStream;

private:
    QString m_completionToAdd;
    QTimer m_completionAddTimer;
    QTimer m_logReadyTimer;
};

#endif // BASEDEVICE_H
