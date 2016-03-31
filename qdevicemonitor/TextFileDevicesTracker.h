#ifndef TEXTFILEDEVICESTRACKER_H
#define TEXTFILEDEVICESTRACKER_H

#include "BaseDevicesTracker.h"

class TextFileDevicesTracker : public BaseDevicesTracker
{
public:
    void openFile(const QString& fileName)
    {
        emit deviceConnected(DataTypes::DeviceType::TextFile, fileName);
    }

private slots:
    void update() override {};
};

#endif // TEXTFILEDEVICESTRACKER_H
