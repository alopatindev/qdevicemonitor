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
};

#endif // SETTINGSDIALOG_H
