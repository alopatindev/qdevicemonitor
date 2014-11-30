#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QDir>
#include <QPointer>
#include <QSettings>
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

    void loadSettings(const QSettings& s);
    void saveSettings(QSettings& s);
};

#endif // SETTINGSDIALOG_H
