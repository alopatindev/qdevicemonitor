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

#include "ui/MainWindow.h"
#include <QApplication>
#include <QTime>
#include <cstdio>

void logOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    (void) type;

    const QByteArray localMsg = msg.toLocal8Bit();
    std::fprintf(
        stdout,
        "%s %s (%s:%u, %s)\n",
        QTime::currentTime().toString().toStdString().c_str(),
        localMsg.constData(),
        context.file,
        context.line,
        context.function
    );
}

int main(int argc, char* argv[])
{
#ifdef QT_DEBUG
    qInstallMessageHandler(logOutput);
#endif

    Q_INIT_RESOURCE(resources);

    QApplication a(argc, argv);
    a.setApplicationName("QDeviceMonitor");
    a.setApplicationVersion(VERSION);
    MainWindow w;
    w.show();

    return a.exec();
}
