#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSerialPort>
#include <vector>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>

#include "serialthread.h"
#include "hit.h"

//Namespaces
using namespace std;

namespace Ui {
class MainWindow;
}

class SettingsDialog;
class RenderArea;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //Constructors
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //Methods
    void printBase();
    void printHit(const Hit &hit);
    void printHistory();


public slots:
    void newHit(Hit hit);
    void serialPortError(QSerialPort::SerialPortError err);
    void changeStatusBar(QString data);
    void openSerialPort();

private slots:
    //Serial connection

    void closeSerialPort();
    void on_actionConfigure_clicked();
    void on_actionHistory_clicked(bool checked);

private:
    Ui::MainWindow *ui;
//    QSerialPort *serial;
    SettingsDialog *settings;

    RenderArea *renderArea;
    SerialThread *m_reader;
    vector<Hit> history;
    qreal multiplier;
    qreal platformSide;
    QGraphicsScene *scene;
    QGraphicsView *view;
    QGraphicsEllipseItem *hitItem;
};

#endif // MAINWINDOW_H
