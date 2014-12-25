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

#include "TextFileDevice.h"
#include "Utils.h"
#include "ThemeColors.h"

#include <QDebug>
#include <QFileInfo>
#include <QHash>
#include <QRegExp>

using namespace DataTypes;

static QStringList s_filesToOpen;

TextFileDevice::TextFileDevice(QPointer<QTabWidget> parent, const QString& id, DeviceType type,
                               const QString& humanReadableName, const QString& humanReadableDescription, QPointer<DeviceAdapter> deviceAdapter)
    : BaseDevice(parent, id, type, humanReadableName, humanReadableDescription, deviceAdapter),
      m_fileChanged(false)
{
    qDebug() << "TextFileDevice::TextFileDevice";
    m_deviceWidget->hideVerbosity();
    m_deviceWidget->onLogFileNameChanged(id);

    connect(&m_fsWatcher, &QFileSystemWatcher::fileChanged, this, [this](const QString& path)
        {
            qDebug() << "fileChanged" << path;
            m_fileChanged = true;
        }
    );

    startLogger();
}

TextFileDevice::~TextFileDevice()
{
    qDebug() << "TextFileDevice::~TextFileDevice";
    stopLogger();
}

void TextFileDevice::startLogger()
{
    qDebug() << "TextFileDevice::startLogger";

    m_logFile.setFileName(m_id);
    m_logFile.open(QIODevice::ReadOnly | QIODevice::Text);

    m_logFileStream = QSharedPointer<QTextStream>(new QTextStream(&m_logFile));
    m_logFileStream->setCodec("UTF-8");

    Utils::seekToLastVisibleLines(m_logFile, *m_logFileStream, m_deviceAdapter->getVisibleBlocks());

    m_fsWatcher.addPath(m_id);
}

void TextFileDevice::stopLogger()
{
    qDebug() << "TextFileDevice::stopLogger";

    m_fsWatcher.removePath(m_id);
    m_logFileStream.clear();
    m_logFile.close();
}

void TextFileDevice::update()
{
    if (m_fileChanged)
    {
        if (!m_logFileStream->atEnd())
        {
            for (int i = 0; i < DeviceAdapter::MAX_LINES_UPDATE && !m_logFileStream->atEnd(); ++i)
            {
                filterAndAddToTextEdit(m_logFileStream->readLine());
            }
        }

        m_fileChanged = !m_logFileStream->atEnd();
    }
}

void TextFileDevice::filterAndAddToTextEdit(const QString& line)
{
    int theme = m_deviceAdapter->isDarkTheme() ? 1 : 0;
    //if (filtersMatch)
    {
        m_deviceWidget->addText(ThemeColors::Colors[theme][ThemeColors::VerbosityVerbose], line + "\n");
    }

    m_deviceWidget->maybeScrollTextEditToEnd();
    //m_deviceWidget->highlightFilterLineEdit(!m_filtersValid);
}

void TextFileDevice::reloadTextEdit()
{
    qDebug() << "reloadTextEdit";
    m_deviceWidget->getTextEdit().clear();
}

void TextFileDevice::maybeAddNewDevicesOfThisType(QPointer<QTabWidget> parent, DevicesMap& map, QPointer<DeviceAdapter> deviceAdapter)
{
    for (const auto& logFile : s_filesToOpen)
    {
        auto it = map.find(logFile);
        if (it == map.end())
        {
            QString fileName = QFileInfo(logFile).fileName();
            map[logFile] = QSharedPointer<BaseDevice>(
                new TextFileDevice(
                    parent,
                    logFile,
                    DeviceType::TextFile,
                    fileName,
                    logFile,
                    deviceAdapter
                )
            );
        }
    }

    s_filesToOpen.clear();
}

void TextFileDevice::openLogFile(const QString& logFile)
{
    s_filesToOpen.append(logFile);
}
