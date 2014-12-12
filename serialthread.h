#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QThread>
#include <QSerialPort>
#include "settingsdialog.h"
#include "hit.h"

class SerialThread : public QThread
{
    Q_OBJECT

public:
    explicit SerialThread(QObject *parent = nullptr);
    ~SerialThread();

    void run() override;
    void setSettings(SettingsDialog::Settings settings);

public slots:
    void quit();

signals:
    void hit(Hit hit);
    void error(QSerialPort::SerialPortError err);

private slots:
    void handleError(QSerialPort::SerialPortError error);

private:
    bool setupSerialPort(QSerialPort *port);

    bool m_quit;
    SettingsDialog::Settings m_settings;
};

#endif // SERIALTHREAD_H
