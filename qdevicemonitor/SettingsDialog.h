#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QDir>
#include <QPointer>
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

public:
    static const QString& getLogsPath();
    static const QString& getConfigPath();
};

#endif // SETTINGSDIALOG_H
