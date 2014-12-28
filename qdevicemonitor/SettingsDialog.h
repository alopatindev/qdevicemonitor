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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QDir>
#include <QPointer>
#include <QSettings>
#include <QSharedPointer>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

    QSharedPointer<Ui::SettingsDialog> ui;

public:
    explicit SettingsDialog(QPointer<QWidget> parent = 0);
    ~SettingsDialog();

    void loadSettings(const QSettings& s);
    void saveSettings(QSettings& s);

public slots:
    void on_editorBrowseButton_clicked();
};

#endif // SETTINGSDIALOG_H
