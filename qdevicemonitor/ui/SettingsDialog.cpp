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
#include <QFileDialog>

SettingsDialog::SettingsDialog(QPointer<QWidget> parent)
    : QDialog(parent)
{
    m_ui = QSharedPointer<Ui::SettingsDialog>::create();
    m_ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::loadSettings(const QSettings& s)
{
    m_ui->visibleBlocksSpinBox->setValue(s.value("visibleBlocks").toInt());
    m_ui->fontComboBox->setCurrentFont(QFont(s.value("font").toString()));
    m_ui->fontSizeSpinBox->setValue(s.value("fontSize").toInt());
    m_ui->fontBoldCheckBox->setChecked(s.value("fontBold").toBool());
    m_ui->darkThemeCheckBox->setChecked(s.value("darkTheme").toBool());
    m_ui->clearAndroidLogCheckBox->setChecked(s.value("clearAndroidLog").toBool());
    m_ui->autoRemoveFilesOlderThanSpinBox->setValue(s.value("autoRemoveFilesHours").toInt());
    m_ui->editorLineEdit->setText(s.value("textEditorPath").toString());
}

void SettingsDialog::saveSettings(QSettings& s)
{
    s.setValue("visibleBlocks", m_ui->visibleBlocksSpinBox->value());
    s.setValue("font", m_ui->fontComboBox->currentFont().family());
    s.setValue("fontSize", m_ui->fontSizeSpinBox->value());
    s.setValue("fontBold", m_ui->fontBoldCheckBox->isChecked());
    s.setValue("darkTheme", m_ui->darkThemeCheckBox->isChecked());
    s.setValue("clearAndroidLog", m_ui->clearAndroidLogCheckBox->isChecked());
    s.setValue("autoRemoveFilesHours", m_ui->autoRemoveFilesOlderThanSpinBox->value());
    s.setValue("textEditorPath", m_ui->editorLineEdit->text());
    s.sync();
}

void SettingsDialog::on_editorBrowseButton_clicked()
{
    qDebug() << "on_editorBrowseButton_clicked";
    const QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        QString(),
#if defined(Q_OS_WIN32)
        tr("Executables (*.exe);;All Files (*)")
#else
        tr("All Files (*)")
#endif
    );

    if (!fileName.isEmpty())
    {
        m_ui->editorLineEdit->setText(fileName);
    }
}
