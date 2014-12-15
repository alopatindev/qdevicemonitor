/*
    This file is part of QDeviceMonitor.

    QDeviceMonitor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QDeviceMonitor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QDeviceMonitor. If not, see <http://www.gnu.org/licenses/>.
*/

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
    ui->visibleBlocksSpinBox->setValue(s.value("visibleBlocks").toInt());
    ui->fontComboBox->setCurrentFont(QFont(s.value("font").toString()));
    ui->fontSizeSpinBox->setValue(s.value("fontSize").toInt());
    ui->darkThemeCheckBox->setChecked(s.value("darkTheme").toBool());
    ui->autoRemoveFilesOlderThanSpinBox->setValue(s.value("autoRemoveFilesHours").toInt());
}

void SettingsDialog::saveSettings(QSettings& s)
{
    s.setValue("visibleBlocks", ui->visibleBlocksSpinBox->value());
    s.setValue("font", ui->fontComboBox->currentFont().family());
    s.setValue("fontSize", ui->fontSizeSpinBox->value());
    s.setValue("darkTheme", ui->darkThemeCheckBox->isChecked());
    s.setValue("autoRemoveFilesHours", ui->autoRemoveFilesOlderThanSpinBox->value());
    s.sync();
}
