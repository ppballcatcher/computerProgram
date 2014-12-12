#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtSerialPort/QSerialPort>
#include <vector>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>

//Namespaces
using namespace std;

namespace Ui {
class MainWindow;
}

class SettingsDialog;
class RenderArea;

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
    double getX() const {return x_;}
    double getY() const {return y_;}
    int getHash() const {return hash_;}

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
    void printHit(const Hit &hit);
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

    RenderArea *renderArea;

    vector<Hit> history;
    qreal multiplier;
    qreal platformSide;
    QGraphicsScene *scene;
    QGraphicsView *view;
    QGraphicsEllipseItem *hitItem;
};

#endif // MAINWINDOW_H
