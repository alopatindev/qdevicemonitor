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

#ifndef IOSDEVICE_H
#define IOSDEVICE_H

#include "BaseDevice.h"
#include <QFile>
#include <QProcess>
#include <QStringList>
#include <QTextStream>

using namespace DataTypes;

class IOSDevice : public BaseDevice
{
    Q_OBJECT

    QProcess m_infoProcess;
    QProcess m_logProcess;
    QFile m_logFile;
    QSharedPointer<QTextStream> m_logFileStream;
    bool m_didReadModel;

    QString m_tempErrorsBuffer;
    QTextStream m_tempErrorsStream;

public:
    explicit IOSDevice(
        QPointer<QTabWidget> parent,
        const QString& id,
        const DeviceType type,
        const QString& humanReadableDescription,
        QPointer<DeviceFacade> deviceFacade
    );
    ~IOSDevice() override;

    void update() override;
    void filterAndAddToTextEdit(const QString& line) override;
    const char* getPlatformName() const override { return "iOS"; }
    void reloadTextEdit() override;

    static void maybeAddNewDevicesOfThisType(QPointer<QTabWidget> parent, DevicesMap& map, QPointer<DeviceFacade> deviceFacade);
    static void releaseTempBuffer();
    static void stopDevicesListProcess();
    static void removedDeviceByTabClose(const QString& id);

private:
    void stopInfoProcess();
    void updateModel();
    void startLogger();
    void stopLogger();
    void checkFilters(bool& filtersMatch, bool& filtersValid, const QStringRef& text);

    void maybeReadErrorsPart();
    void maybeReadLogPart();

private slots:
    void onLogReady() override;
};

#endif // ANDROIDDEVICE_H
