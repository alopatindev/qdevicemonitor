#ifndef ANDROIDDEVICE_H
#define ANDROIDDEVICE_H

#include "BaseDevice.h"

using namespace DataTypes;

class AndroidDevice : public BaseDevice
{
public:
    AndroidDevice(const QString& id, DeviceType type, const QString& humanReadableName, const QString& humanReadableDescription);

    static void addNewDevicesOfThisType(DevicesMap& map);
    virtual qint64 readData(char* data, qint64 maxlen);

private:
    virtual qint64 writeData(const char* data, qint64 len);
};

#endif // ANDROIDDEVICE_H
