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
#include "ui/colors/ColorTheme.h"

#include <QDebug>
#include <QProcess>
#include <QScrollBar>

using namespace DataTypes;

static const QString MARK_LINE("========================== MARK ==========================");

DeviceWidget::DeviceWidget(QPointer<QWidget> parent, QPointer<DeviceFacade> deviceFacade, const QString& id)
    : QWidget(parent)
    , m_deviceFacade(deviceFacade)
    , m_id(id)
{
    m_ui = QSharedPointer<Ui::DeviceWidget>::create();
    m_ui->setupUi(this);

    m_defaultTextEditPalette = m_ui->textEdit->palette();
    m_redPalette = QPalette(Qt::red);
    m_redPalette.setColor(QPalette::Highlight, Qt::red);

    m_textStream.setCodec("UTF-8");
    m_textStream.setString(&m_stringStream, QIODevice::ReadWrite | QIODevice::Text);

    //ui->textEdit->setFontFamily(m_deviceFacade->getFont());
    //ui->textEdit->setFontPointSize(m_deviceFacade->getFontSize());
    m_ui->textEdit->setAttribute(Qt::WA_OpaquePaintEvent);
    m_ui->textEdit->setUndoRedoEnabled(false);
    m_ui->textEdit->document()->setMaximumBlockCount(m_deviceFacade->getVisibleLines());

    clearTextEdit();

    m_ui->verbositySlider->valueChanged(m_ui->verbositySlider->value());
    m_ui->wrapCheckBox->setCheckState(m_ui->wrapCheckBox->isChecked() ? Qt::Checked : Qt::Unchecked);
}

DeviceWidget::~DeviceWidget()
{
    qDebug() << "~DeviceWidget";
}

void DeviceWidget::hideVerbosity()
{
    m_ui->verbositySlider->setVisible(false);
    m_ui->verbosityLabel->setVisible(false);
}

void DeviceWidget::on_verbositySlider_valueChanged(const int value)
{
    qDebug() << "verbosity" << value;
    const char* const v = Verbosity[value];
    m_ui->verbosityLabel->setText(tr(v));
    emit verbosityLevelChanged(value);
}

void DeviceWidget::on_wrapCheckBox_toggled(const bool checked)
{
    m_ui->textEdit->setLineWrapMode(checked ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
    maybeScrollTextEditToEnd();
}

void DeviceWidget::on_scrollLockCheckBox_toggled(const bool)
{
    maybeScrollTextEditToEnd();
}

void DeviceWidget::highlightFilterLineEdit(const bool red)
{
    m_ui->filterLineEdit->setPalette(red ? m_redPalette : m_defaultTextEditPalette);
}

void DeviceWidget::maybeScrollTextEditToEnd()
{
    if (!m_ui->scrollLockCheckBox->isChecked())
    {
        scrollTextEditToEnd();
    }
}

void DeviceWidget::addText(const ColorTheme::ColorType color, const QStringRef& text)
{
    addText(m_deviceFacade->getThemeColor(color), text);
}

void DeviceWidget::addText(const QColor& color, const QStringRef& text)
{
    m_textStream
        << "<font style=\"font-family: " << m_deviceFacade->getFont()
        << "; font-size: " << m_deviceFacade->getFontSize()
        << "pt; font-weight: " << (m_deviceFacade->isFontBold() ? "bold" : "none")
        << ";\" color=\"" << color.name()
        << "\">"
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
        // FIXME: remove this hack
        << text.toString()
#else
        << text
#endif
        << " </font>";
}

void DeviceWidget::flushText()
{
    m_textStream.flush();
    m_ui->textEdit->setUpdatesEnabled(false);
    m_ui->textEdit->append(m_textStream.readAll());
    m_ui->textEdit->setUpdatesEnabled(true);
}

void DeviceWidget::updateTextEditPalette()
{
    QPalette pal;
    if (m_deviceFacade->isDarkTheme())
    {
        pal.setColor(QPalette::Text, Qt::white);
        pal.setColor(QPalette::Base, Qt::black);
    }
    else
    {
        pal = m_defaultTextEditPalette;
    }
    m_ui->textEdit->setPalette(pal);
}

void DeviceWidget::clearTextEdit()
{
    updateTextEditPalette();
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
        const QString textEditor = m_deviceFacade->getTextEditorPath();
        qDebug() << "open" << m_currentLogFileName << "in text editor" << textEditor;
        if (!textEditor.isEmpty())
        {
            QStringList args;
            args.append(m_currentLogFileName);
            QProcess::startDetached(m_deviceFacade->getTextEditorPath(), args);
        }
    }
}

void DeviceWidget::onLogFileNameChanged(const QString& logFileName)
{
    m_currentLogFileName = logFileName;
}

void DeviceWidget::focusFilterInput()
{
    m_ui->filterLineEdit->setFocus();
}

void DeviceWidget::on_markLogButton_clicked()
{
    addText(ColorTheme::VerbosityVerbose, QStringRef(&MARK_LINE));
    m_deviceFacade->writeToLogFile(m_id, MARK_LINE);
    flushText();
}

void DeviceWidget::markLog()
{
    m_ui->markLogButton->click();
}

void DeviceWidget::clearLog()
{
    m_ui->clearLogButton->click();
}

void DeviceWidget::openLogFile()
{
    m_ui->openLogFileButton->click();
}
