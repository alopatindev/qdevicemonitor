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

#include "Utils.h"

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QRegExp>
#include <QDebug>

const char* const Utils::LOGS_DIR = "logs";
const char* const Utils::LOG_EXT = ".log";
const char* const Utils::DATE_FORMAT = "yyyy-MM-dd_hh-mm-ss";

const QString& Utils::getDataPath()
{
    static const QString d = QDir::homePath() + "/." + qApp->applicationName();
    return d;
}

const QString& Utils::getLogsPath()
{
    static const QString d = getDataPath() + "/" + LOGS_DIR;
    static bool initialized = false;
    if (!initialized)
    {
        bool success = QDir(getDataPath()).mkpath(LOGS_DIR);
        qDebug() << "creating directory" << d << "=>" << success;
        initialized = true;
    }
    return d;
}

QString Utils::getNewLogFilePath(const QString& suffix)
{
    return getLogsPath() + "/" + suffix + getCurrentDateTimeUtc() + LOG_EXT;
}

const QString& Utils::getConfigPath()
{
    static const QString d = getDataPath() + "/" + qApp->applicationName() + ".conf";
    return d;
}

QString Utils::removeSpecialCharacters(const QString& text)
{
    QString out(text);
    static QRegExp regexp("[^a-zA-Z\\d\\s]");
    out.remove(regexp);
    out.replace(" ", "_");
    return out;
}

QString Utils::getCurrentDateTimeUtc()
{
    return QDateTime::currentDateTimeUtc().toString(DATE_FORMAT);
}

int Utils::verbosityCharacterToInt(char character)
{
    switch (character)
    {
    case 'V':
        return 5;

    case 'D':
        return 4;

    case 'I':
        return 3;

    case 'W':
        return 2;

    case 'E':
        return 1;

    case 'F':
        return 0;

    default:
        return -1;
    }
}

bool Utils::columnTextMatches(const QString& filter, const QString& text)
{
    static QString f[3];
    f[0] = filter.trimmed();

    if (f[0].isEmpty() || text.indexOf(f[0]) != -1)
    {
        return true;
    }
    else
    {
        f[1] = QString(".*%1.*").arg(f[0]);
        f[2] = QString(".*(%1).*").arg(f[1]);

        static QRegExp rx[] = {
            //QRegExp("", Qt::CaseSensitive, QRegExp::RegExp),
            QRegExp("", Qt::CaseSensitive, QRegExp::RegExp2),
            QRegExp("", Qt::CaseSensitive, QRegExp::Wildcard),
            QRegExp("", Qt::CaseSensitive, QRegExp::WildcardUnix),
            //QRegExp("", Qt::CaseSensitive, QRegExp::FixedString),
            QRegExp("", Qt::CaseSensitive, QRegExp::W3CXmlSchema11)
        };

        for (size_t i = 0; i < sizeof(rx) / sizeof(rx[0]); ++i)
        {
            QRegExp& r = rx[i];
            for (size_t j = 0; j < sizeof(f) / sizeof(f[0]); ++j)
            {
                r.setPattern(f[j]);
                if (r.isValid() && r.exactMatch(text))
                {
                    return true;
                }
            }
        }
        return false;
    }

    return true;
}
