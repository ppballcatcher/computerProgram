#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
//#include "console.h"

#include <QMessageBox>
#include <QtSerialPort/QtSerialPort>


//------------------------------------------------------------------------------------
//
//                                     Constructors
//
//------------------------------------------------------------------------------------

/**
 * @brief MainWindow::MainWindow Constructor that creates the UI and try to stablish
 * a connection with the default parameters when started
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ///Create Serial Port
    serial = new QSerialPort(this);

    ///And the dialog settings
    settings = new SettingsDialog;

    ///Connects the buttons to the slots
    connect(ui->actionConnect, SIGNAL(clicked()), this, SLOT(openSerialPort()));//Connect
    connect(ui->actionConfigure, SIGNAL(clicked()), settings, SLOT(show())); //Configure

    ///Connects
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError))); //If we
            //recieve an error show an error show an error message

    if (serial->open(QIODevice::ReadWrite)){
        ui->statusBar->showMessage("Connected!");
    } else {
        ui->statusBar->showMessage(":(");
    }

    //Print base
    printBase();
}

/**
 * @brief MainWindow::~MainWindow Basic destrctor that close the port when exit
 */
MainWindow::~MainWindow()
{
    closeSerialPort();
    delete serial;
    //delete console;
    delete settings;
    delete ui;
}

//------------------------------------------------------------------------------------
//
//                                     Methods
//
//------------------------------------------------------------------------------------

/**
 * @brief MainWindow::openSerialPort
 */
void MainWindow::openSerialPort()
{
    SettingsDialog::Settings p = settings->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite)) {
            ui->statusBar->showMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                                       .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                                       .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());

        ui->statusBar->showMessage(tr("Open error"));
    }
}

/**
 * @brief MainWindow::closeSerialPort Close the serial port and show it in the Status Bar
 */
void MainWindow::closeSerialPort()
{
    serial->close();
    ui->statusBar->showMessage(tr("Disconnected"));
}

/**
 * @brief MainWindow::readData Send a character that the FPGA is able to understand like "send me
 * information" and the FPGA should response with a the "hash,x,y" of the last hit. Then, generates
 * a new Hit object with that information and save it in the vector.
 */
void MainWindow::readData()
{
    ///Sends the char that the FPGA understands like "send me information".
    ///It should return something like "hash,x,y"
    serial->write("r");

    ///Reads the data and save it in a string
    if (serial->QIODevice::waitForReadyRead(-1)){
        QString data = serial->readAll();
        ///Split the information in the commas
        QStringList dataList = data.split(","); //

        ///Save the information for generate a new Hit object
        QString hashString= dataList.at(0);
        int hash = hashString.toInt();

        QString xString= dataList.at(1);
        double x = xString.toDouble();

        QString yString= dataList.at(2);
        double y = yString.toDouble();

        ///Generate the Hit and save it in the vector
        history.push_back(Hit(hash, x, y));
    }
}


/**
 * @brief MainWindow::handleError Shows an error message with te information from the serial
 * @param error The error that the serial port is sending
 */
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}

/**
 * @brief MainWindow::on_actionConfigure_clicked Private slot to show the settings window
 */
void MainWindow::on_actionConfigure_clicked()
{
    settings->show();
}


void MainWindow::printBase(){


}


void MainWindow::printHit(Hit& hit){

}

void MainWindow::printHistory(){

}
