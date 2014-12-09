#ifndef BASEDEVICE_H
#define BASEDEVICE_H

#include "DeviceWidget.h"
#include "DeviceAdapter.h"
#include "DataTypes.h"

#include <QCompleter>
#include <QPointer>
#include <QProcess>
#include <QString>
#include <QTabWidget>

using namespace DataTypes;

class BaseDevice : public QObject
{
    Q_OBJECT

public:
    explicit BaseDevice(QPointer<QTabWidget> parent, const QString& id, DeviceType type,
                        const QString& humanReadableName, const QString& humanReadableDescription,
                        QPointer<DeviceAdapter> deviceAdapter);
    void updateTabWidget();
    virtual void update() = 0;
    virtual const QCompleter& getFilterCompleter() = 0;

    const QString& getHumanReadableName() const;
    const QString& getHumanReadableDescription() const;

    void setHumanReadableName(const QString& text);
    void setHumanReadableDescription(const QString& text);

    bool isOnline() const;
    void setOnline(bool online);

    const QString& getId() { return m_id; }

signals:

public slots:

protected:
    QString m_id;
    DeviceType m_type;
    QString m_humanReadableName;
    QString m_humanReadableDescription;
    bool m_online;
    QPointer<QTabWidget> m_tabWidget;
    QPointer<DeviceWidget> m_deviceWidget;
    int m_tabIndex;
    QPointer<DeviceAdapter> m_deviceAdapter;
};

#endif // BASEDEVICE_H
