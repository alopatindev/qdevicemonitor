#include "DeviceWidget.h"
#include "DataTypes.h"

#include <QDebug>

using namespace DataTypes;

DeviceWidget::DeviceWidget(QPointer<QWidget> parent)
    : QWidget(parent)
    , ui(new Ui::DeviceWidget)
{
    ui->setupUi(this);
}

void DeviceWidget::on_verbositySlider_valueChanged(int value)
{
    qDebug() << "verbosity" << value;
    Verbosity verbosity = static_cast<Verbosity>(value);
}
