#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_deviceAdapter.setParent(ui->tabWidget);
}

MainWindow::~MainWindow()
{
}
