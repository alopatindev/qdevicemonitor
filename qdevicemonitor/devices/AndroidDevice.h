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

    QProcess m_infoProcess;
    QProcess m_logProcess;
    QProcess m_clearLogProcess;
    QFile m_logFile;
    QSharedPointer<QTextStream> m_logFileStream;
    int m_lastVerbosityLevel;
    bool m_didReadModel;
    bool m_loggerStarted;

public:
    explicit AndroidDevice(
        QPointer<QTabWidget> parent,
        const QString& id,
        const DeviceType type,
        const QString& humanReadableDescription,
        QPointer<DeviceFacade> deviceFacade
    );
    ~AndroidDevice() override;

    void onUpdateFilter(const QString& filter) override;
    void filterAndAddToTextEdit(const QString& line) override;
    const char* getPlatformName() const override { return "Android"; }
    void reloadTextEdit() override;

    void onOnlineChange(const bool online) override;

private slots:
    void onLogReady() override;
    void onUpdateModel();
    void onVerbosityLevelChange(const int level);
    void onLogProcessStatusChange(const QProcess::ProcessState newState);

private:
    void startInfoProcess();
    void stopInfoProcess();
    void startLogProcess();
    void stopLogProcess();

    void startLogger();
    void stopLogger();
    void maybeClearAdbLog();

    void checkFilters(bool& filtersMatch,
                      bool& filtersValid,
                      const VerbosityEnum verbosityLevel = Verbose,
                      const QStringRef& pid = QStringRef(),
                      const QStringRef& tid = QStringRef(),
                      const QStringRef& tag = QStringRef(),
                      const QStringRef& text = QStringRef());
};

#endif // ANDROIDDEVICE_H
