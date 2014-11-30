#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

#include <QDebug>

SettingsDialog::SettingsDialog(QPointer<QWidget> parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::loadSettings(const QSettings& s)
{
    ui->visibleLinesSpinBox->setValue(s.value("visibleLines").toInt());
    ui->fontComboBox->setCurrentFont(QFont(s.value("font").toString()));
    ui->fontSizeSpinBox->setValue(s.value("fontSize").toInt());
    ui->darkThemeCheckBox->setChecked(s.value("darkTheme").toBool());
    ui->autoRemoveFilesOlderThanSpinBox->setValue(s.value("autoRemoveFilesHours").toInt());
}

void SettingsDialog::saveSettings(QSettings& s)
{
    s.setValue("visibleLines", ui->visibleLinesSpinBox->value());
    s.setValue("font", ui->fontComboBox->currentFont().family());
    s.setValue("fontSize", ui->fontSizeSpinBox->value());
    s.setValue("darkTheme", ui->darkThemeCheckBox->isChecked());
    s.setValue("autoRemoveFilesHours", ui->autoRemoveFilesOlderThanSpinBox->value());
    s.sync();
}
