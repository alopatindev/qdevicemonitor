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

#ifndef UTILS_H
#define UTILS_H

#include <QFile>
#include <QString>

#if defined(Q_OS_MAC)
    #include <QThread>
    #include <QDebug>
    #define PRINT_THREAD_ID do { qDebug() <<  __func__ << "thread" << QThread::currentThreadId(); } while(0);
#else
    #define PRINT_THREAD_ID
#endif

namespace Utils
{
    static const char* const LOGS_DIR = "logs";
    static const char* const LOG_EXT = ".log";
    static const char* const DATE_FORMAT = "yyyy-MM-dd_hh-mm-ss";

    const QString& getDataPath();
    const QString& getLogsPath();
    QString getNewLogFilePath(const QString& suffix);
    const QString& getConfigPath();
    QString removeSpecialCharacters(const QString& text);
    QString getCurrentDateTimeUtc();
    int verbosityCharacterToInt(const char character);
}

#endif // UTILS_H
