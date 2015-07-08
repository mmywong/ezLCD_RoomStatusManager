#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Room.h"
#include <iostream>
#include <list>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    int room_number;
    std::list<Room> mylist;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void set_room_number(int number);
    void add_room(Room room);

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
