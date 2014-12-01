#include "DeviceWidget.h"
#include "DataTypes.h"

#include <QDebug>
#include <QPalette>

using namespace DataTypes;

DeviceWidget::DeviceWidget(QPointer<QWidget> parent, QPointer<DeviceAdapter> deviceAdapter)
    : QWidget(parent)
    , ui(new Ui::DeviceWidget)
    , m_deviceAdapter(deviceAdapter)
{
    ui->setupUi(this);

    if (m_deviceAdapter->isDarkTheme())
    {
        //ui->textEdit->setTextBackgroundColor(Qt::black);
        QPalette pal;
        pal.setColor(QPalette::Text, Qt::white);
        pal.setColor(QPalette::Base, Qt::black);
        ui->textEdit->setPalette(pal);
    }

    //ui->textEdit->setDefaultFont(QFont(m_deviceAdapter->getFont(), m_deviceAdapter->getFontSize()));
    ui->textEdit->setFontFamily(m_deviceAdapter->getFont());
    ui->textEdit->setFontPointSize(m_deviceAdapter->getFontSize());

    ui->verbositySlider->setValue(ui->verbositySlider->value());
    ui->wrapCheckBox->setChecked(ui->wrapCheckBox->isChecked());
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
