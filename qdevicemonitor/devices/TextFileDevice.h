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

using namespace DataTypes;

class TextFileDevice : public BaseDevice
{
    Q_OBJECT

    QProcess m_tailProcess;
    bool m_loggerStarted;

public:
    explicit TextFileDevice(
        QPointer<QTabWidget> parent,
        const QString& id,
        const DeviceType type,
        const QString& humanReadableDescription,
        QPointer<DeviceFacade> deviceFacade
    );
    ~TextFileDevice() override;

    void onUpdateFilter(const QString& filter) override;
    void filterAndAddToTextEdit(const QString& line) override;
    const char* getPlatformName() const override { return "Text File"; }
    void reloadTextEdit() override;

private:
    void startLogger();
    void stopLogger();

    void checkFilters(bool& filtersMatch, bool& filtersValid, const QStringView text);

private slots:
    void onLogReady() override;
};

#endif // ANDROIDDEVICE_H
