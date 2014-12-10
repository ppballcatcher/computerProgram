#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtSerialPort/QSerialPort>
#include <vector>
#include <QGraphicsScene>

//Namespaces
using namespace std;

namespace Ui {
class MainWindow;
}

class SettingsDialog;

class Hit
{
public:
    //Constructors
    Hit(int hash, double x, double y){
        hash_ = hash;
        x_ = x;
        y_ = y;
    }

    ~Hit(){}

    //Methods
    unsigned char getX(){return x_;}
    unsigned char getY(){return y_;}
    unsigned char getHash(){return hash_;}

private:
    double x_;
    double y_;
    int hash_;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //Constructors
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //Methods
    void printBase();
    void printHit(Hit& hit);
    void printHistory();

private slots:
    //Serial connection
    void openSerialPort();
    void closeSerialPort();
    void readData();

    void handleError(QSerialPort::SerialPortError error);

    void on_actionConfigure_clicked();



private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    SettingsDialog *settings;

    QGraphicsScene *scene;

    vector<Hit> history;
};

#endif // MAINWINDOW_H
