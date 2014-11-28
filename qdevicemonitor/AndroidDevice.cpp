#include "AndroidDevice.h"

AndroidDevice::AndroidDevice()
    : m_logLinesDidRead(false)
{
}

bool AndroidDevice::isReady() const
{
    // TODO
    return true;
}

const QStringList& AndroidDevice::readLogLines() const
{
    return m_logLines;
}
