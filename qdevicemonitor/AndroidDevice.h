#ifndef ANDROIDDEVICE_H
#define ANDROIDDEVICE_H

#include <IDevice.h>

class AndroidDevice : public IDevice
{
    QStringList m_logLines;
    bool m_logLinesDidRead;

public:
    AndroidDevice();
    bool isReady() const;
    const QStringList& readLogLines() const;
};

#endif // ANDROIDDEVICE_H
