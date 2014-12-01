#ifndef ANDROIDDEVICE_H
#define ANDROIDDEVICE_H

#include "BaseDevice.h"
#include <QFile>
#include <QProcess>
#include <QTextStream>

using namespace DataTypes;

class AndroidDevice : public BaseDevice
{
    QProcess m_deviceInfoProcess;
    QProcess m_deviceLogProcess;
    QFile m_deviceLogFile;
    QSharedPointer<QTextStream> m_deviceLogFileStream;
    bool m_emptyTextEdit;
    int m_lastVerbosityLevel;

public:
    explicit AndroidDevice(QPointer<QTabWidget> parent, const QString& id, DeviceType type,
                           const QString& humanReadableName, const QString& humanReadableDescription,
                           QPointer<DeviceAdapter> deviceAdapter);
    ~AndroidDevice();
    virtual void update();

    static void addNewDevicesOfThisType(QPointer<QTabWidget> parent, DevicesMap& map, QPointer<DeviceAdapter> deviceAdapter);
    static void stopDevicesListProcess();

private:
    void updateDeviceModel();
    void startLogger();
    void stopLogger();
    void filterAndAddToTextEdit(const QString& line);
    void reloadTextEdit();
};

#endif // ANDROIDDEVICE_H
