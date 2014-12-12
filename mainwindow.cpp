#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "serialthread.h"
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
    hitItem(nullptr)
{
    ui->setupUi(this);

    ///And the dialog settings
    settings = new SettingsDialog;

    ///Connects the buttons to the slots
    connect(ui->actionHistory, SIGNAL(clicked(bool)),
            this, SLOT(on_actionHistory_clicked(bool))); //Print history

    connect(ui->actionConfigure, SIGNAL(clicked()),
            settings, SLOT(show())); //Configure

    connect(settings, SIGNAL(updateSettingsSignal()),
            this, SLOT(openSerialPort()));

    m_reader=nullptr;
    openSerialPort();

    multiplier = 0.0555;
    platformSide = 450;

    scene = new QGraphicsScene;
    scene->setSceneRect(0, 0, platformSide, platformSide);
    view = new QGraphicsView(scene);
    ui->drawingLayout->addWidget(view);

    //Print base
    printBase();
}

/**
 * @brief MainWindow::~MainWindow Basic destrctor that close the port when exit
 */
MainWindow::~MainWindow()
{
    m_reader->quit();
    m_reader->wait();
    closeSerialPort();
//    delete serial;
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
    if (m_reader){
        m_reader->quit();
        m_reader->wait();
        delete m_reader;
    }
    m_reader = new SerialThread(this);
    connect(m_reader, &SerialThread::error, this, &MainWindow::serialPortError);
    connect(m_reader, &SerialThread::changeStatusBar, this, &MainWindow::changeStatusBar);
    connect(m_reader, &SerialThread::hit, this, &MainWindow::newHit);
    m_reader->setSettings(settings->settings());
    m_reader->start();
}

/**
 * @brief MainWindow::closeSerialPort Close the serial port and show it in the Status Bar
 */
void MainWindow::closeSerialPort()
{
//    serial->close();
    ui->statusBar->showMessage(tr("Disconnected"));
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

void MainWindow::newHit(Hit hit)
{
    history.push_back(hit);
    printHit(hit);
    qDebug() << "Hit:" << hit.getHash() << "," << hit.getX() << "," << hit.getY();
}

void MainWindow::serialPortError(QSerialPort::SerialPortError err)
{
    qDebug() << err; // TODO string
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

/**
 * @brief MainWindow::changeStatusBar
 * @param data
 */
void MainWindow::changeStatusBar(QString data)
{
    ui->statusBar->showMessage(data);
}
