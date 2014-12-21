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

#include <QString>

class Utils
{
public:
    static const char* const LOGS_DIR;
    static const char* const LOG_EXT;
    static const char* const DATE_FORMAT;

    static const QString& getDataPath();
    static const QString& getLogsPath();
    static QString getNewLogFilePath(const QString& suffix);
    static const QString& getConfigPath();
    static QString removeSpecialCharacters(const QString& text);
    static QString getCurrentDateTimeUtc();
    static int verbosityCharacterToInt(char character);
    static bool columnTextMatches(const QString& filter, const QString& text);
};

#endif // UTILS_H
