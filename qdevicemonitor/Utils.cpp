#include "Utils.h"

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QRegExp>

const QString& Utils::getDataPath()
{
    static const QString d = QDir::homePath() + "/" + qApp->applicationName();
    return d;
}

const QString& Utils::getLogsPath()
{
    static const QString LOGS_DIR = "logs";
    static const QString d = QDir::homePath() + "/" + qApp->applicationName() + "/" + LOGS_DIR;

    static bool initialized = false;
    if (!initialized)
    {
        QDir(QDir::homePath()).mkdir(qApp->applicationName());
        QDir(QDir::homePath() + "/" + qApp->applicationName()).mkdir(LOGS_DIR);
        initialized = true;
    }

    return d;
}

QString Utils::getNewLogFilePath(const QString& suffix)
{
    return getLogsPath() + "/" + suffix + "_" + getCurrentDateTime() + ".log";
}

const QString& Utils::getConfigPath()
{
    static const QString d = getDataPath() + "/" + qApp->applicationName() + ".conf";
    return d;
}

QString Utils::removeSpecialCharacters(const QString& text)
{
    QString out(text);
    out.remove(QRegExp("[^a-zA-Z\\d\\s]"));
    return out;
}

QString Utils::getCurrentDateTime()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
}
