#ifndef DEVICEWIDGET_H
#define DEVICEWIDGET_H

#include "ui_DeviceWidget.h"
#include "DeviceAdapter.h"

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
    QPointer<DeviceAdapter> m_deviceAdapter;

public:
    explicit DeviceWidget(QPointer<QWidget> parent, QPointer<DeviceAdapter> deviceAdapter);
    QLineEdit& getFilterLineEdit() const { return *(ui->filterLineEdit); }
    QTextEdit& getTextEdit() const { return *(ui->textEdit); }
    int getVerbosityLevel() const;
    void maybeScrollTextEditToEnd();

signals:

public slots:
    void on_verbositySlider_valueChanged(int value);
    void on_wrapCheckBox_toggled(bool checked);
    void on_scrollLockCheckBox_toggled(bool checked);

private:
    void scrollTextEditToEnd();
};

#endif // DEVICEWIDGET_H
