#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"
#include "DeviceAdapter.h"

#include <QMainWindow>
#include <QPointer>
#include <QSharedPointer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    DeviceAdapter m_deviceAdapter;

public:
    explicit MainWindow(QPointer<QWidget> parent = 0);
    ~MainWindow();

    void loadSettings();
    void saveSettings();

public slots:
    void on_actionSettings_triggered();

private:
    QSharedPointer<Ui::MainWindow> ui;
};

#endif // MAINWINDOW_H
