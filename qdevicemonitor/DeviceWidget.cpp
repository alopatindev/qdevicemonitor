#include "DeviceWidget.h"

DeviceWidget::DeviceWidget(QPointer<QWidget> parent)
    : QWidget(parent)
    , ui(new Ui::DeviceWidget)
{
    ui->setupUi(this);
}
