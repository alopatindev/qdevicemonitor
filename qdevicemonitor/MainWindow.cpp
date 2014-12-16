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

#include "MainWindow.h"
#include "SettingsDialog.h"
#include "Utils.h"

#include <QDebug>
#include <QMessageBox>
#include <QSettings>

MainWindow::MainWindow(QPointer<QWidget> parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    (void)Utils::getLogsPath();
    loadSettings();
    m_deviceAdapter.setParent(ui->tabWidget);
    m_deviceAdapter.start();
}

MainWindow::~MainWindow()
{
    saveSettings();
    m_deviceAdapter.stop();
}

void MainWindow::on_actionSettings_triggered()
{
    QSettings s(Utils::getConfigPath(), QSettings::IniFormat);
    saveSettings();
    SettingsDialog dialog(this);
    dialog.loadSettings(s);
    if (dialog.exec() == QDialog::Accepted)
    {
        dialog.saveSettings(s);
        loadSettings();
        m_deviceAdapter.allDevicesReloadText();
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this,
        tr("About %1 %2").arg(qApp->applicationName(),
                              qApp->applicationVersion()),
        tr("<b>%1 %2</b><br><br>"
           "Crossplatform Android, iOS and text file log viewer written in C++/Qt.<br>\n"
           "Copyright (c) 2014 Alexander Lopatin<br>\n"
           "<a href=\"https://github.com/alopatindev/qdevicemonitor\">https://github.com/alopatindev/qdevicemonitor</a><br>\n\n"
           "<center>This program is released under<br>\n"
           "the terms of the<br>\n"
           "GNU GENERAL PUBLIC LICENSE<br>\n"
           "Version 3, 29 June 2007</center>")
                .arg(qApp->applicationName(),
                     qApp->applicationVersion()));
}

void MainWindow::on_actionAboutQt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::loadSettings()
{
    qDebug() << "loadSettings";
    QSettings s(Utils::getConfigPath(), QSettings::IniFormat);
    qDebug() << "path" << s.fileName();

    QRect geom = s.value("geometry").toRect();
    if (geom.isValid())
    {
        setGeometry(geom);
        qDebug() << geom;
    }

    m_deviceAdapter.loadSettings(s);
}

void MainWindow::saveSettings()
{
    qDebug() << "saveSettings";
    QSettings s(Utils::getConfigPath(), QSettings::IniFormat);

    s.setValue("geometry", geometry());
    qDebug() << geometry();
    m_deviceAdapter.saveSettings(s);

    s.sync();
}
