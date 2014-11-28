#ifndef DEVICEADAPTER_H
#define DEVICEADAPTER_H

#include <QObject>

class DeviceAdapter : public QObject
{
    Q_OBJECT
public:
    explicit DeviceAdapter(QObject* parent = 0);

signals:

public slots:

};

#endif // DEVICEADAPTER_H
