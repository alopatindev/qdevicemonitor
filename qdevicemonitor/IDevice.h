#ifndef IDEVICE_H
#define IDEVICE_H

#include <QStringList>

class IDevice
{
public:
    virtual bool isReady() const = 0;
    virtual const QStringList& readLogLines() const = 0;
};

#endif // IDEVICE_H
