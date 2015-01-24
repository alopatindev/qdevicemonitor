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

#ifndef DEVICEWIDGET_H
#define DEVICEWIDGET_H

#include "ui_DeviceWidget.h"
#include "DeviceAdapter.h"

#include <QPointer>
#include <QSharedPointer>
#include <QTextStream>
#include <QWidget>

namespace Ui {
class DeviceWidget;
}

class DeviceWidget : public QWidget
{
    Q_OBJECT

    QSharedPointer<Ui::DeviceWidget> ui;
    QPointer<DeviceAdapter> m_deviceAdapter;
    QTextStream m_textStream;
    QString m_stringStream;
    QString m_currentLogFileName;

public:
    explicit DeviceWidget(QPointer<QWidget> parent, QPointer<DeviceAdapter> deviceAdapter);
    virtual ~DeviceWidget();
    void hideVerbosity();

    inline QLineEdit& getFilterLineEdit() const { return *(ui->filterLineEdit); }
    inline QTextEdit& getTextEdit() const { return *(ui->textEdit); }
    inline int getVerbosityLevel() const { return ui->verbositySlider->value(); }
    void highlightFilterLineEdit(bool red);
    void maybeScrollTextEditToEnd();
    void addText(const QColor& color, const QString& text);
    void clearTextEdit();
    void onLogFileNameChanged(const QString& logFileName);
    void focusFilterInput();

signals:

public slots:
    void on_verbositySlider_valueChanged(int value);
    void on_wrapCheckBox_toggled(bool checked);
    void on_scrollLockCheckBox_toggled(bool checked);
    void on_openLogFileButton_clicked();

private:
    void scrollTextEditToEnd();
};

#endif // DEVICEWIDGET_H
