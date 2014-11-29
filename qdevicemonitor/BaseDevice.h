#ifndef BASEDEVICE_H
#define BASEDEVICE_H

#include "DeviceAdapter.h"
#include "DataTypes.h"

#include <QIODevice>
#include <QProcess>
#include <QString>

using namespace DataTypes;

class BaseDevice : public QIODevice
{
    Q_OBJECT

public:
    BaseDevice(const QString& id, DeviceType type, const QString& humanReadableName, const QString& humanReadableDescription);

    const QString& getHumanReadableName() const;
    const QString& getHumanReadableDescription() const;

    bool isOnline() const;

signals:

public slots:

private:
    QString m_id;
    DeviceType m_type;

protected:
    QString m_humanReadableName;
    QString m_humanReadableDescription;
    bool m_online;
};

#endif // BASEDEVICE_H
