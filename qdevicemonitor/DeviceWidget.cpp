#include "DeviceWidget.h"
#include "DataTypes.h"

#include <QDebug>

using namespace DataTypes;

DeviceWidget::DeviceWidget(QPointer<QWidget> parent)
    : QWidget(parent)
    , ui(new Ui::DeviceWidget)
{
    ui->setupUi(this);
    ui->verbositySlider->valueChanged(ui->verbositySlider->value());
    ui->wrapCheckBox->toggled(ui->wrapCheckBox->isChecked());
}

void DeviceWidget::on_verbositySlider_valueChanged(int value)
{
    qDebug() << "verbosity" << value;
    const char* const v = Verbosity[value];
    ui->verbosityLabel->setText(tr(v));
}

void DeviceWidget::on_wrapCheckBox_toggled(bool checked)
{
    ui->textEdit->setLineWrapMode(checked ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
}

int DeviceWidget::getVerbosityLevel() const
{
    return ui->verbositySlider->value();
}
