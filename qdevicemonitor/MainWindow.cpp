#include "MainWindow.h"
#include "SettingsDialog.h"
#include "Utils.h"

#include <QDebug>
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
    saveSettings();
    SettingsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        loadSettings();
    }
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
