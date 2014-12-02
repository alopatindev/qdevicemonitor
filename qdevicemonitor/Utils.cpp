#include "Utils.h"

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QRegExp>
#include <QDebug>

static const QString LOGS_DIR = "logs";

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
    return getLogsPath() + "/" + suffix + getCurrentDateTime() + ".log";
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
    return out;
}

QString Utils::getCurrentDateTime()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
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
