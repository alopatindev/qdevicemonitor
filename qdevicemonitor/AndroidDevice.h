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

public:
    explicit AndroidDevice(QPointer<QTabWidget> parent, const QString& id, DeviceType type,
                           const QString& humanReadableName, const QString& humanReadableDescription,
                           QPointer<DeviceAdapter> deviceAdapter);
    ~AndroidDevice();
    virtual void update();

    static void addNewDevicesOfThisType(QPointer<QTabWidget> parent, DevicesMap& map, QPointer<DeviceAdapter> deviceAdapter);

private:
    void updateDeviceModel();
    void startLogger();
};

#endif // ANDROIDDEVICE_H
