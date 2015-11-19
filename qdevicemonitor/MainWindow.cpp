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
#include "TextFileDevice.h"
#include "Utils.h"

#include <cstdlib>
#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QFileInfo>
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

void MainWindow::on_actionOpen_triggered()
{
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), m_lastLogDirectory, tr("Logs (*.log *.log.*);;All Files (*)"));
    if (!fileName.isNull())
    {
        const QFileInfo fi(fileName);
        m_lastLogDirectory = fi.absolutePath();
        TextFileDevice::openLogFile(fi.absoluteFilePath());
    }
}

void MainWindow::on_actionQuit_triggered()
{
    qDebug() << "on_actionQuit_triggered";
    close();
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
           "Copyright (c) 2014—2015 Alexander Lopatin<br>\n"
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
    if (index != -1)
    {
        m_deviceAdapter.removeDeviceByTabIndex(index);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        switch (event->key())
        {
        case Qt::Key_F:
            m_deviceAdapter.focusFilterInput();
            break;
        case Qt::Key_E:
            m_deviceAdapter.openLogFile();
            break;
        case Qt::Key_M:
            m_deviceAdapter.markLog();
            break;
        default:
            break;
        }
    }
    else if (event->modifiers() & Qt::AltModifier)
    {
        switch (event->key())
        {
        case Qt::Key_C:
            m_deviceAdapter.clearLog();
            break;
        default:
            break;
        }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MidButton)
    {
        const QTabBar& tabBar = *(ui->tabWidget->tabBar());
        const QPoint mousePos = tabBar.mapFrom(this, event->pos());
        const int index = tabBar.tabAt(mousePos);
        on_tabWidget_tabCloseRequested(index);
    }
}

void MainWindow::loadSettings()
{
    qDebug() << "loadSettings";
    const QSettings s(Utils::getConfigPath(), QSettings::IniFormat);
    qDebug() << "config path" << s.fileName();

    QVariant geom = s.value("geometry");
    if (geom.isValid())
    {
        setGeometry(geom.toRect());
    }
    else
    {
        QRect geom = QApplication::desktop()->screenGeometry();
        int screenWidth = geom.width();
        int screenHeight = geom.height();
        geom.setWidth(int(screenWidth * 0.7f));
        geom.setHeight(int(screenHeight * 0.7f));
        geom.moveLeft(int(screenWidth * 0.5f - geom.width() * 0.5f));
        geom.moveTop(int(screenHeight * 0.5f - geom.height() * 0.5f));
        setGeometry(geom);
    }

    const QVariant lastLogDirectory = s.value("lastLogDirectory");
    if (lastLogDirectory.isValid())
    {
        m_lastLogDirectory = lastLogDirectory.toString();
    }
    else
    {
#ifdef Q_OS_WIN32
        m_lastLogDirectory = "C:/";
#else
        m_lastLogDirectory = "/var/log";
#endif
    }

    m_deviceAdapter.loadSettings(s);
}

void MainWindow::saveSettings()
{
    qDebug() << "saveSettings";
    QSettings s(Utils::getConfigPath(), QSettings::IniFormat);

    s.setValue("geometry", geometry());
    s.setValue("lastLogDirectory", m_lastLogDirectory);
    m_deviceAdapter.saveSettings(s);

    s.sync();
}

void MainWindow::setupEnvironment()
{
    qDebug() << "MainWindow::setupEnvironment";

    (void) Utils::getLogsPath();

#if defined(Q_OS_MAC)
    const QString thirdPartyDir(QCoreApplication::applicationDirPath() + "/3rdparty");
    if (QFileInfo(thirdPartyDir).isDir())
    {
        const QStringList thirdPartyProgramDirs = QDir(thirdPartyDir).entryList(QStringList(), QDir::AllDirs | QDir::NoDotAndDotDot);
        const char* pPath = std::getenv("PATH");
        pPath = pPath ? pPath : "";
        QString path(pPath);
        const QString dyldFallbackLibraryPath(std::getenv("DYLD_FALLBACK_LIBRARY_PATH"));
        for (const auto& i : thirdPartyProgramDirs)
        {
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
        (void) ::setenv("PATH", path.toStdString().c_str(), 1);
        (void) ::setenv("DYLD_FALLBACK_LIBRARY_PATH", dyldFallbackLibraryPath.toStdString().c_str(), 1);
    }
#elif defined(Q_OS_WIN32)
    const QString thirdPartyDir(QCoreApplication::applicationDirPath() + "\\3rdparty\\bin");
    if (QFileInfo(thirdPartyDir).isDir())
    {
        const char* pPath = std::getenv("Path");
        pPath = pPath ? pPath : "";
        QString path(pPath);
        const QString prefix(path.isEmpty() ? "" : ";");
        path = QString("Path=%1%2%3").arg(path).arg(prefix).arg(thirdPartyDir);
        qDebug() << "Path" << path;
        (void) ::putenv(const_cast<char*>(path.toStdString().c_str()));
    }
#endif
}

void MainWindow::checkExternalPrograms()
{
    static const size_t n = 5;
    static const char* programs[n] = {
        "adb",
        "idevice_id",
        "ideviceinfo",
        "idevicesyslog",
        "tail",
    };

#if defined(Q_OS_UNIX)
    static const char* whichProgram = "which";
#elif defined(Q_OS_WIN32)
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
