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

#ifndef ANDROIDDEVICE_H
#define ANDROIDDEVICE_H

#include "BaseDevice.h"
#include <QFile>
#include <QProcess>
#include <QStringList>
#include <QTextStream>

using namespace DataTypes;

class AndroidDevice : public BaseDevice
{
    Q_OBJECT

    QProcess m_deviceInfoProcess;
    QProcess m_deviceLogProcess;
    QProcess m_deviceClearLogProcess;
    QFile m_deviceLogFile;
    QSharedPointer<QTextStream> m_deviceLogFileStream;
    int m_lastVerbosityLevel;
    bool m_didReadDeviceModel;

public:
    explicit AndroidDevice(QPointer<QTabWidget> parent, const QString& id, const DeviceType type,
                           const QString& humanReadableName, const QString& humanReadableDescription,
                           QPointer<DeviceAdapter> deviceAdapter);
    ~AndroidDevice() override;
    void update() override;
    void filterAndAddToTextEdit(const QString& line) override;
    const char* getPlatformString() const override { return getPlatformStringStatic(); }
    void reloadTextEdit() override;

    static void maybeAddNewDevicesOfThisType(QPointer<QTabWidget> parent, DevicesMap& map, QPointer<DeviceAdapter> deviceAdapter);
    static void stopDevicesListProcess();
    static void removedDeviceByTabClose(const QString& id);
    void onOnlineChange(const bool online) override;

private:
    void updateDeviceModel();
    void startLogger();
    void stopLogger();
    void maybeClearAdbLog();

    bool columnMatches(const QString& column, const QString& filter, const QString& originalValue, bool& filtersValid, bool& columnFound) const;
    void checkFilters(bool& filtersMatch, bool& filtersValid, const QStringList& filters,
                      const VerbosityEnum verbosityLevel = Verbose,
                      const QStringRef& pid = QStringRef(),
                      const QStringRef& tid = QStringRef(),
                      const QStringRef& tag = QStringRef(),
                      const QStringRef& text = QStringRef());
    static const char* getPlatformStringStatic() { return "Android"; }
};

#endif // ANDROIDDEVICE_H
