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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"
#include "devices/DeviceFacade.h"

#include <QKeyEvent>
#include <QMainWindow>
#include <QPointer>
#include <QSharedPointer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QSharedPointer<DeviceFacade> m_deviceFacade;
    QString m_lastLogDirectory;

public:
    explicit MainWindow(QPointer<QWidget> parent = 0);
    ~MainWindow() override;

    void loadSettings();
    void saveSettings();

public slots:
    void on_actionOpen_triggered();
    void on_actionDetectDevices_triggered();
    void on_actionQuit_triggered();
    void on_actionSettings_triggered();
    void on_actionAbout_triggered();
    void on_actionAboutQt_triggered();
    void on_tabWidget_tabCloseRequested(const int index);

protected:
    void keyReleaseEvent(QKeyEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
#if defined(Q_OS_WIN32)
    bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
#endif

private:
    QSharedPointer<Ui::MainWindow> m_ui;
    void checkExternalPrograms();
    void setupEnvironment();
};

#endif // MAINWINDOW_H
