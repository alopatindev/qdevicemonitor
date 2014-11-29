#ifndef DEVICEWIDGET_H
#define DEVICEWIDGET_H

#include "ui_DeviceWidget.h"

#include <QPointer>
#include <QSharedPointer>
#include <QWidget>

namespace Ui {
class DeviceWidget;
}

class DeviceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceWidget(QPointer<QWidget> parent = 0);

signals:

public slots:

private:
    QSharedPointer<Ui::DeviceWidget> ui;
};

#endif // DEVICEWIDGET_H
