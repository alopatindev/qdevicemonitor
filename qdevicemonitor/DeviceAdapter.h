#ifndef DEVICEADAPTER_H
#define DEVICEADAPTER_H

#include "DataTypes.h"

#include <QObject>
#include <QPointer>
#include <QTabWidget>
#include <QTimer>
#include <QSettings>

class DeviceAdapter : public QObject
{
    Q_OBJECT

    static const int UPDATE_FREQUENCY = 100;

private:
    DataTypes::DevicesMap m_devicesMap;
    QTimer m_updateTimer;

    int m_visibleLines;
    QString m_font;
    int m_fontSize;
    bool m_darkTheme;
    int m_autoRemoveFilesHours;

public:
    explicit DeviceAdapter(QPointer<QTabWidget> parent = 0);
    void start();
    void stop();
    void loadSettings(const QSettings& s);
    void saveSettings(QSettings& s);

    bool isDarkTheme() const { return m_darkTheme; }
    const QString& getFont() const { return m_font; }
    int getFontSize() const { return m_fontSize; }
    int getAutoRemoveFilesHours() { return m_autoRemoveFilesHours; }

signals:

public slots:
    void update();

private:
    void updateDevicesMap();
};

#endif // DEVICEADAPTER_H
