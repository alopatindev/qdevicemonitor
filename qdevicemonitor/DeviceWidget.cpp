#include "DeviceWidget.h"
#include "DataTypes.h"

#include <QDebug>
#include <QPalette>
#include <QScrollBar>

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

    ui->verbositySlider->valueChanged(ui->verbositySlider->value());
    ui->wrapCheckBox->setCheckState(ui->wrapCheckBox->isChecked() ? Qt::Checked : Qt::Unchecked);
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
    maybeScrollTextEditToEnd();
}

void DeviceWidget::on_scrollLockCheckBox_toggled(bool)
{
    maybeScrollTextEditToEnd();
}

int DeviceWidget::getVerbosityLevel() const
{
    return ui->verbositySlider->value();
}

void DeviceWidget::highlightFilterLineEdit(bool red)
{
    static QPalette normalPal = ui->filterLineEdit->palette();
    static QPalette redPal(Qt::red);
    redPal.setColor(QPalette::Highlight, Qt::red);

    ui->filterLineEdit->setPalette(red ? redPal : normalPal);
}

void DeviceWidget::maybeScrollTextEditToEnd()
{
    if (!ui->scrollLockCheckBox->isChecked())
    {
        scrollTextEditToEnd();
    }
}

void DeviceWidget::scrollTextEditToEnd()
{
    QScrollBar& sb = *(getTextEdit().verticalScrollBar());
    if (sb.maximum() > 0)
    {
        sb.setValue(sb.maximum());
    }
}
