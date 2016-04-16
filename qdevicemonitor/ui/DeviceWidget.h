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
#include "devices/DeviceFacade.h"
#include "ui/colors/ColorTheme.h"

#include <QPalette>
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

    QSharedPointer<Ui::DeviceWidget> m_ui;
    QPalette m_defaultTextEditPalette;
    QPalette m_redPalette;
    QPointer<DeviceFacade> m_deviceFacade;
    QTextStream m_textStream;
    QString m_stringStream;
    QString m_currentLogFileName;

public:
    explicit DeviceWidget(QPointer<QWidget> parent, QPointer<DeviceFacade> deviceFacade);
    ~DeviceWidget() override;
    void hideVerbosity();

    inline QLineEdit& getFilterLineEdit() const { return *(m_ui->filterLineEdit); }
    inline QTextEdit& getTextEdit() const { return *(m_ui->textEdit); }
    inline int getVerbosityLevel() const { return m_ui->verbositySlider->value(); }
    void highlightFilterLineEdit(bool red);
    void maybeScrollTextEditToEnd();
    void addText(const ColorTheme::ColorType color, const QStringRef& text);
    void addText(const QColor& color, const QStringRef& text);
    void flushText();
    void clearTextEdit();
    void onLogFileNameChanged(const QString& logFileName);
    void focusFilterInput();
    void markLog();
    void clearLog();
    void openLogFile();

signals:
    void verbosityLevelChanged(const int level);

public slots:
    void on_verbositySlider_valueChanged(const int value);
    void on_wrapCheckBox_toggled(const bool checked);
    void on_scrollLockCheckBox_toggled(const bool checked);
    void on_openLogFileButton_clicked();
    void on_markLogButton_clicked();

private:
    void updateTextEditPalette();
    void scrollTextEditToEnd();
};

#endif // DEVICEWIDGET_H
