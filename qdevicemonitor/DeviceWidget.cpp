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

#include "DeviceWidget.h"
#include "DataTypes.h"
#include "ThemeColors.h"

#include <QDebug>
#include <QPalette>
#include <QProcess>
#include <QScrollBar>

using namespace DataTypes;

const QString MARK_LINE("========================== MARK ==========================");

DeviceWidget::DeviceWidget(QPointer<QWidget> parent, QPointer<DeviceAdapter> deviceAdapter)
    : QWidget(parent)
    , m_deviceAdapter(deviceAdapter)
{
    ui = QSharedPointer<Ui::DeviceWidget>::create();
    ui->setupUi(this);

    m_textStream.setCodec("UTF-8");
    m_textStream.setString(&m_stringStream, QIODevice::ReadWrite | QIODevice::Text);

    //ui->textEdit->setFontFamily(m_deviceAdapter->getFont());
    //ui->textEdit->setFontPointSize(m_deviceAdapter->getFontSize());
    ui->textEdit->setUndoRedoEnabled(false);
    ui->textEdit->document()->setMaximumBlockCount(m_deviceAdapter->getVisibleLines());

    clearTextEdit();

    ui->verbositySlider->valueChanged(ui->verbositySlider->value());
    ui->wrapCheckBox->setCheckState(ui->wrapCheckBox->isChecked() ? Qt::Checked : Qt::Unchecked);
}

DeviceWidget::~DeviceWidget()
{
    qDebug() << "~DeviceWidget";
}

void DeviceWidget::hideVerbosity()
{
    ui->verbositySlider->setVisible(false);
    ui->verbosityLabel->setVisible(false);
}

void DeviceWidget::on_verbositySlider_valueChanged(int value)
{
    qDebug() << "verbosity" << value;
    const char* const v = Verbosity[value];
    ui->verbosityLabel->setText(tr(v));
}

void DeviceWidget::on_wrapCheckBox_toggled(bool checked)
{
    ui->textEdit->setLineWrapMode(checked ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
    maybeScrollTextEditToEnd();
}

void DeviceWidget::on_scrollLockCheckBox_toggled(bool)
{
    maybeScrollTextEditToEnd();
}

void DeviceWidget::highlightFilterLineEdit(bool red)
{
    static QPalette normalPal = ui->filterLineEdit->palette();
    static QPalette redPal(Qt::red);
    redPal.setColor(QPalette::Highlight, Qt::red);

    ui->filterLineEdit->setPalette(red ? redPal : normalPal);
}

void DeviceWidget::maybeScrollTextEditToEnd()
{
    if (!ui->scrollLockCheckBox->isChecked())
    {
        scrollTextEditToEnd();
    }
}

void DeviceWidget::addText(const QColor& color, const QStringRef& text)
{
    m_textStream
        << "<font style=\"font-family: " << m_deviceAdapter->getFont()
        << "; font-size: " << m_deviceAdapter->getFontSize()
        << "pt; font-weight: " << (m_deviceAdapter->isFontBold() ? "bold" : "none")
        << ";\" color=\"" << color.name()
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
        // FIXME: remove this hack
        << "\">" << QString().append(text)
#else
        << "\">" << text
#endif
        << " </font>";
}

void DeviceWidget::flushText()
{
    m_textStream.flush();
    ui->textEdit->setUpdatesEnabled(false);
    ui->textEdit->append(m_textStream.readLine());
    ui->textEdit->setUpdatesEnabled(true);
}

void DeviceWidget::clearTextEdit()
{
    static QPalette defaultPal = ui->textEdit->palette();
    QPalette pal;
    if (m_deviceAdapter->isDarkTheme())
    {
        pal.setColor(QPalette::Text, Qt::white);
        pal.setColor(QPalette::Base, Qt::black);
    }
    else
    {
        pal = defaultPal;
    }
    ui->textEdit->setPalette(pal);

    getTextEdit().clear();
}

void DeviceWidget::scrollTextEditToEnd()
{
    QScrollBar& sb = *(getTextEdit().verticalScrollBar());
    if (sb.maximum() > 0)
    {
        sb.setValue(sb.maximum());
    }
}

void DeviceWidget::on_openLogFileButton_clicked()
{
    if (!m_currentLogFileName.isEmpty())
    {
        const QString textEditor = m_deviceAdapter->getTextEditorPath();
        qDebug() << "open" << m_currentLogFileName << "in text editor" << textEditor;
        if (!textEditor.isEmpty())
        {
            QStringList args;
            args.append(m_currentLogFileName);
            QProcess::startDetached(m_deviceAdapter->getTextEditorPath(), args);
        }
    }
}

void DeviceWidget::onLogFileNameChanged(const QString& logFileName)
{
    m_currentLogFileName = logFileName;
}

void DeviceWidget::focusFilterInput()
{
    ui->filterLineEdit->setFocus();
}

void DeviceWidget::on_markLogButton_clicked()
{
    const int themeIndex = m_deviceAdapter->isDarkTheme() ? 1 : 0;
    addText(ThemeColors::Colors[themeIndex][ThemeColors::VerbosityVerbose], QStringRef(&MARK_LINE));
    //addToLogBuffer(MARK_LINE);
}

void DeviceWidget::markLog()
{
    ui->markLogButton->click();
}

void DeviceWidget::clearLog()
{
    ui->clearLogButton->click();
}

void DeviceWidget::openLogFile()
{
    ui->openLogFileButton->click();
}
