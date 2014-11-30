#ifndef THEMECOLORS_H
#define THEMECOLORS_H

#include <QtGlobal>

namespace ThemeColors
{
    enum Themes
    {
        NormalTheme,
        DarkTheme
    };

    enum ColorTypes
    {
        VerbosityFatal,
        VerbosityError,
        VerbosityWarn,
        VerbosityInfo,
        VerbosityDebug,
        VerbosityVerbose,

        DateTime,
        Pid,
        Tid,
        Tag
    };

    static Qt::GlobalColor Colors[][10] = {
        {
            Qt::red,
            Qt::red,
            Qt::darkYellow,
            Qt::darkGreen,
            Qt::blue,
            Qt::black,

            Qt::black,
            Qt::darkBlue,
            Qt::blue,
            Qt::darkGreen
        },
        {
            Qt::red,
            Qt::red,
            Qt::yellow,
            Qt::green,
            Qt::blue,
            Qt::white,

            Qt::white,
            Qt::blue,
            Qt::darkBlue,
            Qt::green
        },
    };
}

#endif // THEMECOLORS_H
