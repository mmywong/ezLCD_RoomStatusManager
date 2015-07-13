#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QString>
#include <QDebug>
#include <QDesktopWidget>
#include <QTimer>
#include <QtNetwork/QUdpSocket>
#include <QAbstractTableModel>
#include <QTableView>
#include <time.h>

const int SCK_SIZE = 4;

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

    void on_changeStatusIdle_clicked();
    void on_changeStatusDND_clicked();
    void on_changeStatusHK_clicked();
    void comm();
    void on_viewStatusButton_clicked();
    void on_viewRoomButton_clicked();
    void on_viewStaffButton_clicked();
    void on_viewModeButton_clicked();
    void on_editModeButton_clicked();

private:
    Ui::MainWindow *ui;
    QTimer timer;
    QUdpSocket *socket;
    QUdpSocket *socketList[2];
};

#endif // MAINWINDOW_H
