#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "room.h"
#include <QHBoxLayout>


int checkStatus = 1;
int checkStaff = 0;
int checkRoom = 0;
int state = 100;
int selectIdle = 0;
int selectDND = 0;
int selectHK = 0;
int buttonChange = 0;
QString IDLE = "Idle";
QString DND = "Do Not Disturb";
QString HK = "House Keeping";
QString status = IDLE;
QString ip = "172.21.42.56";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setRowCount(50);
    ui->tableWidget->setShowGrid(false);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->horizontalHeader()->setVisible(false);

    for (int c = 0; c < ui->tableWidget->horizontalHeader()->count(); ++c)
    {
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
    }

    displayRooms();

    /*
    connect(&timer, SIGNAL(timeout()), this, SLOT(comm()));
    timer.start(100);

    socket = new QUdpSocket(this);
    */
}



//michelle's
void MainWindow::displayRooms()
{
    //temp rooms:
    Room room1 = Room(100, 0, "maid 1", 1600);
    Room room2 = Room(101, 0, "maid 2", 1600);
    Room room3 = Room(102, 1, "maid 3", 1600);
    Room room4 = Room(103, 0, "maid 4", 1000);
    Room room5 = Room(104, 3, "maid 1", 1600);
    Room room6 = Room(105, 0, "maid 2", 1600);
    Room roomlist[] = {room1,room2,room3,room4,room5,room6};

    //going to call this using Rooms objects...
    for(int i = 0; i < 6; i++)
    {
        ui->tableWidget->setItem(i,0, new QTableWidgetItem(status));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem("Room #" + roomlist[i].getNumber()));
        ui->tableWidget->setItem(i,2, new QTableWidgetItem("Employee #" + QString::number(roomlist[i].getInCharge())));
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

/* from Derek; note: commented out are buttons not yet made
void MainWindow::on_changeStatusIdle_clicked()
{

    ui->changeStatusIdle->setStyleSheet("background-color:yellow");
    ui->changeStatusDND->setStyleSheet("background-color:white");
    ui->changeStatusHK->setStyleSheet("background-color:white");
    ui->tableWidget->setCurrentItem(ui->tableWidget->item(0,0));
    if(ui->tableWidget->currentItem()->isSelected()){
        ui->tableWidget->currentItem()->setBackgroundColor(Qt::yellow);
    }
    state = 10;
    qDebug() << "Idle button";
}

void MainWindow::on_changeStatusDND_clicked()
{
    if(selectDND == 0){
        selectDND = 1;
        selectHK = 0;
        ui->changeStatusDND->setStyleSheet("background-color:red");
        ui->changeStatusIdle->setStyleSheet("background-color:white");
        ui->changeStatusHK->setStyleSheet("background-color:white");
    }
    else if(selectDND == 1){
        selectDND = 0;
        selectHK = 0;
        ui->changeStatusDND->setStyleSheet("background-color:white");
        ui->changeStatusHK->setStyleSheet("background-color:white");
        ui->changeStatusIdle->setStyleSheet("background-color:yellow");
    }
    ui->tableWidget->setCurrentItem(ui->tableWidget->item(0,0));
    if(ui->tableWidget->currentItem()->isSelected()){
        ui->tableWidget->currentItem()->setBackgroundColor(Qt::red);
    }
    state = 30;
    qDebug() << "DND button";
}

void MainWindow::on_changeStatusHK_clicked()
{
    if(selectHK == 0){
        selectHK = 1;
        selectDND = 0;
        ui->changeStatusHK->setStyleSheet("background-color:green");
        ui->changeStatusIdle->setStyleSheet("background-color:white");
        ui->changeStatusDND->setStyleSheet("background-color:white");
    }
    else if(selectHK == 1){
        selectHK = 0;
        selectDND = 0;
        ui->changeStatusHK->setStyleSheet("background-color:white");
        ui->changeStatusDND->setStyleSheet("background-color:white");
        ui->changeStatusIdle->setStyleSheet("background-color:yellow");
    }
    ui->tableWidget->setCurrentItem(ui->tableWidget->item(0,0));
    if(ui->tableWidget->currentItem()->isSelected()){
        ui->tableWidget->currentItem()->setBackgroundColor(Qt::green);
    }
    state = 40;
    qDebug() << "HK button";
}
*/

/*
void MainWindow::comm()
{
    // for QT to send packets to Arduino
    QByteArray datagram;
    if(state == 100)
    {
        datagram = QByteArray::number(100);
        socket->writeDatagram(datagram.data(), datagram.size(), QHostAddress(ip), 99);
    }
    else if(state == 10)
    {
        ui->tableWidget->item(0,0)->setBackgroundColor(Qt::yellow);
        datagram = QByteArray::number(10);
        socket->writeDatagram(datagram.data(), datagram.size(), QHostAddress(ip), 99);
    }
    else if(state == 30)
    {
        ui->tableWidget->item(0,0)->setBackgroundColor(Qt::red);
        datagram = QByteArray::number(30);
        socket->writeDatagram(datagram.data(), datagram.size(), QHostAddress(ip), 99);
    }
    else if(state == 40)
    {
        ui->tableWidget->item(0,0)->setBackgroundColor(Qt::green);
        datagram = QByteArray::number(40);
        socket->writeDatagram(datagram.data(), datagram.size(), QHostAddress(ip), 99);
    }

    // for QT to read packets from Arduino
    if(socket->hasPendingDatagrams()) {
        datagram.resize(socket->pendingDatagramSize());
        socket->readDatagram(datagram.data(), datagram.size());

        if(QString(datagram.data()).toInt() == 40){
            ui->tableWidget->item(0,0)->setBackgroundColor(Qt::green);
            ui->tableWidget->item(0,0)->setText("House Keeping");

        }
        else if(QString(datagram.data()).toInt() == 30){
            ui->tableWidget->item(0,0)->setBackgroundColor(Qt::red);
            ui->tableWidget->item(0,0)->setText("Do Not Disturb");
        }
        else if(QString(datagram.data()).toInt() == 10){
            ui->tableWidget->item(0,0)->setBackgroundColor(Qt::yellow);
            ui->tableWidget->item(0,0)->setText("Idle");
        }
        qDebug() << "read datagram.data().toInt() = " << QString(datagram.data()).toInt();
    }
}
*/

void MainWindow::on_viewStatusButton_clicked(){
   if(checkStatus == 0){
       checkStatus = 1;
       ui->viewStatusButton->setStyleSheet("background-color: rgb(85, 255, 255)");
       ui->viewRoomButton->setStyleSheet("background-color:white");
       ui->viewStaffButton->setStyleSheet("background-color:white");
   }
   else if(checkStatus == 1){
       checkStatus = 0;
       state = 100;
       ui->viewStatusButton->setStyleSheet("background-color:white");
       ui->viewRoomButton->setStyleSheet("background-color:white");
       ui->viewStaffButton->setStyleSheet("background-color:white");
   }



}

void MainWindow::on_viewRoomButton_clicked(){
   if(checkRoom == 0){
       checkRoom = 1;
       ui->viewStatusButton->setStyleSheet("background-color:white");
       ui->viewRoomButton->setStyleSheet("background-color: rgb(85, 255, 255)");
       ui->viewStaffButton->setStyleSheet("background-color:white");
   }
   else if(checkRoom == 1){
       checkRoom = 0;
       state = 100;
       ui->viewStatusButton->setStyleSheet("background-color:white");
       ui->viewRoomButton->setStyleSheet("background-color:white");
       ui->viewStaffButton->setStyleSheet("background-color:white");
   }
}

void MainWindow::on_viewStaffButton_clicked(){
   if(checkStaff == 0){
       checkStaff = 1;
       ui->viewStatusButton->setStyleSheet("background-color:white");
       ui->viewRoomButton->setStyleSheet("background-color:white");
       ui->viewStaffButton->setStyleSheet("background-color: rgb(85, 255, 255)");
   }
   else if(checkStaff == 1){
       checkStaff = 0;
       state = 100;
       ui->viewStatusButton->setStyleSheet("background-color:white");
       ui->viewRoomButton->setStyleSheet("background-color:white");
       ui->viewStaffButton->setStyleSheet("background-color:white");
   }
}
