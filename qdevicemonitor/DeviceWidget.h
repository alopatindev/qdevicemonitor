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

    QSharedPointer<Ui::DeviceWidget> ui;

public:
    explicit DeviceWidget(QPointer<QWidget> parent = 0);
    QTextEdit& getTextEdit() const { return *(ui->textEdit); }

signals:

public slots:
    void on_verbositySlider_valueChanged(int value);
};

#endif // DEVICEWIDGET_H
