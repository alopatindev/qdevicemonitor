#ifndef ANDROIDDEVICE_H
#define ANDROIDDEVICE_H

#include "BaseDevice.h"
#include <QFile>
#include <QProcess>
#include <QTextStream>
#include <QTimer>

using namespace DataTypes;

class AndroidDevice : public BaseDevice
{
    Q_OBJECT

    QProcess m_deviceInfoProcess;
    QProcess m_deviceLogProcess;
    QFile m_deviceLogFile;
    QSharedPointer<QTextStream> m_deviceLogFileStream;
    bool m_emptyTextEdit;
    int m_lastVerbosityLevel;
    QString m_lastFilter;
    bool m_didReadModel;
    QTimer m_reloadTextEditTimer;

public:
    explicit AndroidDevice(QPointer<QTabWidget> parent, const QString& id, DeviceType type,
                           const QString& humanReadableName, const QString& humanReadableDescription,
                           QPointer<DeviceAdapter> deviceAdapter);
    ~AndroidDevice();
    virtual void update();

    static void addNewDevicesOfThisType(QPointer<QTabWidget> parent, DevicesMap& map, QPointer<DeviceAdapter> deviceAdapter);
    static void stopDevicesListProcess();

private:
    void updateDeviceModel();
    void startLogger();
    void stopLogger();

    bool columnMatches(const QString& column, const QString& filter, const QString& originalValue, bool& filtersValid) const;
    bool columnTextMatches(const QString& filter, const QString& text, bool& filtersValid) const;
    void checkFilters(bool& filtersMatch, bool& filtersValid, const QStringList& filters,
                      VerbosityEnum verbosityLevel = Verbose,
                      const QString& pid = QString(),
                      const QString& tid = QString(),
                      const QString& tag = QString(),
                      const QString& text = QString()) const;
    void filterAndAddToTextEdit(const QString& line);
    void scheduleReloadTextEdit(int timeout = 500);

public slots:
    void reloadTextEdit();
};

#endif // ANDROIDDEVICE_H
