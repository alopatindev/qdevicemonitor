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

#include <cstdlib>
#include <QDebug>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QStringList>

MainWindow::MainWindow(QPointer<QWidget> parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupEnvironment();
    loadSettings();
    checkExternalPrograms();

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

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    qDebug() << "MainWindow::on_tabWidget_tabCloseRequested" << index;
    m_deviceAdapter.removeDeviceByTabIndex(index);
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

void MainWindow::setupEnvironment()
{
    qDebug() << "MainWindow::setupEnvironment";

    (void)Utils::getLogsPath();

#ifdef Q_OS_MAC
    QString thirdPartyDir(QCoreApplication::applicationDirPath() + "/3rdparty");
    if (QFileInfo(thirdPartyDir).isDir())
    {
        const QStringList thirdPartyProgramDirs = QDir(thirdPartyDir).entryList(QStringList(), QDir::AllDirs | QDir::NoDotAndDotDot);

        QString path(std::getenv("PATH"));
        QString dyldFallbackLibraryPath(std::getenv("DYLD_FALLBACK_LIBRARY_PATH"));
        for (const auto& i : thirdPartyProgramDirs)
        {
            qDebug() << "MainWindow::setupEnvironment 1" << i;
            const QString prefix(path.isEmpty() ? "" : ":");
            const QString dir = QString("%1/%2").arg(thirdPartyDir).arg(i);
            path += QString("%1%2/bin").arg(prefix).arg(dir);
            if (QFileInfo(QString("%1/lib").arg(dir)).isDir())
            {
                const QString prefix(dyldFallbackLibraryPath.isEmpty() ? "" : ":");
                dyldFallbackLibraryPath += QString("%1%2/lib").arg(prefix).arg(dir);
            }
        }
        qDebug() << "PATH" << path;
        qDebug() << "DYLD_FALLBACK_LIBRARY_PATH" << dyldFallbackLibraryPath;
        ::setenv("PATH", path.toStdString().c_str(), 1);
        ::setenv("DYLD_FALLBACK_LIBRARY_PATH", dyldFallbackLibraryPath.toStdString().c_str(), 1);
    }
#endif
}

void MainWindow::checkExternalPrograms()
{
    static const size_t n = 3;
    static const char* programs[n] = {
        "adb",
        "idevice_id",
        "idevicesyslog"
    };

#ifdef Q_OS_UNIX
    static const char* whichProgram = "which";
#endif
#ifdef Q_OS_WIN32
    static const char* whichProgram = "where";
#endif

    QProcess procs[n];

    for (size_t i = 0; i < n; ++i)
    {
        QStringList args;
        args.append(programs[i]);
        procs[i].start(whichProgram, args);
    }

    for(size_t i = 0; i < n; ++i)
    {
        procs[i].waitForFinished();
        if (procs[i].exitCode() != 0)
        {
            (void) QMessageBox::warning(
                this,
                tr("Some functionality is unavailable"),
                tr("Program \"%1\" is not found. Please add its directory path to PATH environment variable and restart the application.")
                    .arg(programs[i]));
        }
    }
}
