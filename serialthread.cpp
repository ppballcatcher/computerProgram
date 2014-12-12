#include <QStringList>
#include <QDebug>
#include <iostream>
#include "serialthread.h"


SerialThread::SerialThread(QObject *parent)
    : QThread(parent)
    , m_quit(false)
{
}

SerialThread::~SerialThread()
{
}

void SerialThread::run()
{
    qDebug() << "Starting thread";
    char buf[256];
    qint64 len;
    int id_prev = -1;
    QSerialPort *port = new QSerialPort;

    if (!setupSerialPort(port)) {
        return;
    }

    connect(port, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>
            (&QSerialPort::error),
            this, &SerialThread::handleError);

    while (!m_quit) {
        port->write("r");

        if (port->waitForReadyRead(-1)) {
            len = port->readLine(buf, sizeof(buf));

            if (len > 0) {
                QString data(buf);
                qDebug() << data;
                data.remove('\n');
                QStringList dataList = data.split(",", QString::SkipEmptyParts);

                // Check for sane data. If OK, generate the Hit and emit to GUI thread
                if (dataList.size() == 3) {
                    bool ok[3] = {true};

                    QString idStr = dataList.at(0);
                    int id = idStr.toInt(&ok[0]);

                    QString xString = dataList.at(1);
                    double x = xString.toDouble(&ok[1]);

                    QString yString = dataList.at(2);
                    double y = yString.toDouble(&ok[2]);

                    if (ok[0] && ok[1] && ok[2] && id != id_prev) {
                        id_prev = id;
                        emit hit(Hit(id, x, y));
                    }
                }
            }
        }

        msleep(400);
    }

    port->close();
    qDebug() << "RW thread exiting";
    exit();
}

void SerialThread::setSettings(SettingsDialog::Settings settings)
{
    m_settings = settings;
}

void SerialThread::quit()
{
    m_quit = true;
}

/**
 * @brief MainWindow::handleError Shows an error message with te information from the serial
 * @param error The error that the serial port is sending
 */
void SerialThread::handleError(QSerialPort::SerialPortError err)
{
    emit error(err);
    quit();
}

/**
 * @brief SerialThread::setupSerialPort
 * @param port
 * @return
 */
bool SerialThread::setupSerialPort(QSerialPort *port)
{
    port->setPortName(m_settings.name);
    port->setBaudRate(m_settings.baudRate);
    port->setDataBits(m_settings.dataBits);
    port->setParity(m_settings.parity);
    port->setStopBits(m_settings.stopBits);
    port->setFlowControl(m_settings.flowControl);

    if (port->open(QIODevice::ReadWrite)) {
        emit(changeStatusBar(QString(tr("Connected to %1 : %2, %3, %4, %5, %6")
                            .arg(m_settings.name)
                            .arg(m_settings.stringBaudRate)
                            .arg(m_settings.stringDataBits)
                            .arg(m_settings.stringParity)
                            .arg(m_settings.stringStopBits)
                            .arg(m_settings.stringFlowControl)
                            )
                        )
                );
        return true;
    } else {
        emit(changeStatusBar(QString(tr(":("))));
        return false;
    }
}
