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

#ifndef TEXTFILEDEVICE_H
#define TEXTFILEDEVICE_H

#include "BaseDevice.h"
#include <QFile>
#include <QProcess>
#include <QStringList>
#include <QTextStream>

using namespace DataTypes;

class TextFileDevice : public BaseDevice
{
    Q_OBJECT

    QProcess m_tailProcess;

public:
    explicit TextFileDevice(QPointer<QTabWidget> parent, const QString& id, const DeviceType type,
                           const QString& humanReadableName, const QString& humanReadableDescription,
                           QPointer<DeviceAdapter> deviceAdapter);
    ~TextFileDevice() override;
    void update() override;
    void filterAndAddToTextEdit(const QString& line) override;
    const char* getPlatformString() const override { return getPlatformStringStatic(); }
    void reloadTextEdit() override;

    static void maybeAddNewDevicesOfThisType(QPointer<QTabWidget> parent, DevicesMap& map, QPointer<DeviceAdapter> deviceAdapter);
    static void openLogFile(const QString& logFile);

private:
    void startLogger();
    void stopLogger();

    static const char* getPlatformStringStatic() { return "Text File"; }

    void checkFilters(bool& filtersMatch, bool& filtersValid, const QStringList& filters, const QString& text) const;
};

#endif // ANDROIDDEVICE_H
