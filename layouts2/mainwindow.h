#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "room.h"
#include <QMainWindow>
#include <QApplication>
#include <QPushButton>
#include <QString>
#include <QDebug>
#include <QTimer>
#include <QtNetwork/QUdpSocket>
#include <time.h>
#include <sstream>
#include <string>
#include <iostream>

const int SCK_SIZE = 2;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void changeStatusOption();
    void communicate_Qt_Arduino();
    void changeViewOption();
    void ModeButton_clicked();
    void on_comboBoxFilter_activated(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QTimer timer;
    QUdpSocket *socketList[SCK_SIZE];
};

#endif // MAINWINDOW_H
