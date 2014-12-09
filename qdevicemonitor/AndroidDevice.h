#ifndef ANDROIDDEVICE_H
#define ANDROIDDEVICE_H

#include "BaseDevice.h"
#include <QFile>
#include <QProcess>
#include <QStandardItemModel>
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
    QCompleter m_filterCompleter;
    QStandardItemModel m_filterCompleterModel;
    QTimer m_completionAddTimer;
    static const int COMPLETION_ADD_TIMEOUT = 10 * 1000;

public:
    explicit AndroidDevice(QPointer<QTabWidget> parent, const QString& id, DeviceType type,
                           const QString& humanReadableName, const QString& humanReadableDescription,
                           QPointer<DeviceAdapter> deviceAdapter);
    ~AndroidDevice();
    virtual void update();
    virtual const QCompleter& getFilterCompleter();

    static void addNewDevicesOfThisType(QPointer<QTabWidget> parent, DevicesMap& map, QPointer<DeviceAdapter> deviceAdapter);
    static void stopDevicesListProcess();

private:
    void updateDeviceModel();
    void startLogger();
    void stopLogger();

    bool columnMatches(const QString& column, const QString& filter, const QString& originalValue, bool& filtersValid, bool& columnFound) const;
    bool columnTextMatches(const QString& filter, const QString& text) const;
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
    void addFilterAsCompletion();
};

#endif // ANDROIDDEVICE_H
