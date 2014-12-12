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
    ui(new Ui::MainWindow),
    hitItem(0)
{
    ui->setupUi(this);

    ///Create Serial Port
    serial = new QSerialPort(this);

    ///And the dialog settings
    settings = new SettingsDialog;

    ///Connects the buttons to the slots
    connect(ui->actionHistory, SIGNAL(clicked(bool)),
            this, SLOT(on_actionHistory_clicked(bool))); //Print history

    connect(ui->actionConfigure, SIGNAL(clicked()),
            settings, SLOT(show())); //Configure

    ///Connects
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError))); //If we
            //receive an error show an error show an error message

    openSerialPort();

    multiplier = 0.0555;
    platformSide = 450;

    scene = new QGraphicsScene;
    scene->setSceneRect(0, 0, platformSide, platformSide);
    view = new QGraphicsView(scene);
    ui->drawingLayout->addWidget(view);

    //Print base
    printBase();
    printHit(Hit(1,300,300));
    printHit(Hit(1,300,200));
}

/**
 * @brief MainWindow::~MainWindow Basic destrctor that close the port when exit
 */
MainWindow::~MainWindow()
{
    closeSerialPort();
    delete serial;
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
qDebug() << "asasd";
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
        printHit(*(history.end()-1));
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

/**
 * @brief MainWindow::printBase Print the base and the sensors
 */
void MainWindow::printBase()
{
    //QPen pen()
    scene->addEllipse(platformSide * multiplier, platformSide * multiplier, platformSide * multiplier, platformSide * multiplier);
    scene->addEllipse(platformSide * (1 - 2 * multiplier), platformSide * multiplier, platformSide * multiplier, platformSide * multiplier);
    scene->addEllipse(platformSide * (1 - 2 * multiplier), platformSide * (1 - 2 * multiplier), platformSide * multiplier, platformSide * multiplier);
    scene->addEllipse(platformSide * multiplier, platformSide * (1 - 2 * multiplier), platformSide * multiplier, platformSide * multiplier);
}

/**
 * @brief MainWindow::printHit Print a hit
 * @param hit The hit to be printed
 */
void MainWindow::printHit(Hit const& hit)
{
    if (!hitItem) {
        hitItem = new QGraphicsEllipseItem(0, 0, multiplier * platformSide, multiplier * platformSide);
        scene->addItem(hitItem);
    }

    double x = hit.getX();
    double y = hit.getY();
    hitItem->setPos(x, y);
    history.push_back(hit);
}

/**
 * @brief MainWindow::on_actionHistory_clicked Shows all the hits when clicked and clear the
 * screen when clicked again
 */
void MainWindow::on_actionHistory_clicked(bool checked)
{
    if (checked){
        scene->clear();
        printBase();

        for (auto const &hit : history) {
            QGraphicsEllipseItem *item = new QGraphicsEllipseItem(hit.getX(), hit.getY(), multiplier * platformSide, multiplier * platformSide);
            item->setBrush(Qt::black);
            scene->addItem(item);
        }

        ui->actionHistory->setChecked(true);
    } else {
        scene->clear();
        printBase();
        ui->actionHistory->setChecked(false);
    }
}
