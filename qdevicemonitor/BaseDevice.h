#ifndef BASEDEVICE_H
#define BASEDEVICE_H

#include "DeviceAdapter.h"
#include "DataTypes.h"

#include <QIODevice>
#include <QPointer>
#include <QProcess>
#include <QString>
#include <QTabWidget>

using namespace DataTypes;

class BaseDevice : public QIODevice
{
    Q_OBJECT

public:
    BaseDevice(QObject* parent, const QString& id, DeviceType type, const QString& humanReadableName, const QString& humanReadableDescription);
    void updateTabWidget();
    virtual void update() = 0;

    const QString& getHumanReadableName() const;
    const QString& getHumanReadableDescription() const;

    bool isOnline() const;
    void setOnline(bool online);

signals:

public slots:

protected:
    QString m_id;
    DeviceType m_type;
    QString m_humanReadableName;
    QString m_humanReadableDescription;
    bool m_online;
    QPointer<QTabWidget> m_tabWidget;
    int m_tabIndex;
};

#endif // BASEDEVICE_H
