#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

SettingsDialog::SettingsDialog(QPointer<QWidget> parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
}

const QString& SettingsDialog::getLogsPath()
{
    static const QString d = QDir::homePath() + "/" + qApp->applicationName();
    return d;
}

const QString& SettingsDialog::getConfigPath()
{
    static const QString d = getLogsPath() + "/" + qApp->applicationName() + ".conf";
    return d;
}
