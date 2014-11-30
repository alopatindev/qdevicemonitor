#ifndef UTILS_H
#define UTILS_H

#include <QString>

class Utils
{
public:
    static const QString& getDataPath();
    static const QString& getLogsPath();
    static QString getNewLogFilePath(const QString& suffix);
    static const QString& getConfigPath();
    static QString removeSpecialCharacters(const QString& text);
    static QString getCurrentDateTime();
    static int verbosityCharacterToInt(char character);
};

#endif // UTILS_H
