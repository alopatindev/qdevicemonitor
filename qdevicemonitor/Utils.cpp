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
#include <QDebug>
#include <QRegExp>
#include <QRegularExpression>
#include <QtCore/QStringBuilder>

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
        const bool success = QDir(getDataPath()).mkpath(LOGS_DIR);
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
    static const QRegExp regexp("[^a-zA-Z\\d\\s]");
    out.remove(regexp);
    out.replace(" ", "_");
    return out;
}

QString Utils::getCurrentDateTimeUtc()
{
    return QDateTime::currentDateTimeUtc().toString(DATE_FORMAT);
}

int Utils::verbosityCharacterToInt(const char character)
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

bool Utils::columnMatches(const QString& column, const QString& filter, const QStringRef& originalValue, bool& filtersValid, bool& columnFound)
{
    if (filter.startsWith(column))
    {
        columnFound = true;
        const QString value = filter.mid(column.length());
        if (value.isEmpty())
        {
            filtersValid = false;
        }
        else if (!originalValue.contains(value))
        {
            return false;
        }
    }
    return true;
}

bool Utils::columnTextMatches(const QString& filter, const QStringRef& text)
{
    const QString textFilter = filter.trimmed();

    if (textFilter.isEmpty() || text.indexOf(textFilter) != -1)
    {
        return true;
    }
    else
    {
        static QRegularExpression re;
        const QString regexpFilter(".*(" % textFilter % ").*");
        const bool dirty = re.pattern() != regexpFilter;
        if (dirty)
        {
            re.setPattern(regexpFilter);
            re.setPatternOptions(QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
        }

        QRegularExpressionMatch match = re.match(text);
        return match.hasMatch();
    }

    return true;
}
