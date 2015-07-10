#include "mainwindow.h"
#include "ui_mainwindow.h"

// flags
int checkViewMode = 1;
int checkEditMode = 0;
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
QString status_states[3] = {IDLE, DND, HK};
QString ip[5] = {"172.21.42.56", "172.21.42.57"};


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // intiailize configuration for tableWidget
    ui->tableWidget->setColumnCount(4); // number of columns in table
    ui->tableWidget->setRowCount(20);   // number of rows in table
    ui->tableWidget->setShowGrid(false);    // hide the grid lines
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);    // disable editing for table cells
    ui->tableWidget->horizontalHeader()->setVisible(false); // hide the horizontal headers at the top of the table
    ui->tableWidget->setSelectionMode(QTableWidget::NoSelection);   // hide the highlighting of a cell when a cell is selected

    // resize the columns to evenly spread out along the whole size of the table
    for (int c = 0; c < ui->tableWidget->horizontalHeader()->count(); ++c)  // horizontalHeader()->count() is the total number of headers/columns
    {
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(c, QHeaderView::Stretch);
    }

    srand(time(NULL));  // for generating random employee ID number

    // initialize random status of rooms, room numbers, and random employee IDs
    for(int i = 100; i < 120; i++)  // randomly chose room numbers ranging from 100-119
    {
        int staffID = rand()%99999; // maximum of a 5-digit random number
        int roomStatus = rand()%3;  // randomly select 1 of the 3 room statuses: (Idle, Do Not Disturb, or House Keeping)

        // create tableWidget Items (cells)
        ui->tableWidget->setItem(i%20,0, new QTableWidgetItem(status_states[roomStatus]));
        ui->tableWidget->setItem(i%20,1,new QTableWidgetItem("Room #" + QString::number(i)));
        ui->tableWidget->setItem(i%20,2, new QTableWidgetItem("Employee #" + QString::number(staffID)));

        // change the background color of the cell respective to each room status
        if(ui->tableWidget->item(i%20,0)->text() == "Idle"){
            ui->tableWidget->item(i%20,0)->setBackgroundColor(Qt::yellow);
        }
        else if(ui->tableWidget->item(i%20,0)->text() == "Do Not Disturb"){
            ui->tableWidget->item(i%20,0)->setBackgroundColor(Qt::red);
        }
        else if(ui->tableWidget->item(i%20,0)->text() == "House Keeping"){
            ui->tableWidget->item(i%20,0)->setBackgroundColor(Qt::green);
        }
    }

    // connect the timer to the comm() function to constantly read packet data sent from Arduinos to Qt
    connect(&timer, SIGNAL(timeout()), this, SLOT(comm()));
    timer.start(100);

    for(int i = 0; i < 2; i++){
        socketList[i] = new QUdpSocket(this);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_changeStatusIdle_clicked()
{
    if(checkEditMode == 1){
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
}

void MainWindow::on_changeStatusDND_clicked()
{
    if(checkEditMode == 1){
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
}

void MainWindow::on_changeStatusHK_clicked()
{
    if(checkEditMode == 1){
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
}

void MainWindow::comm()
{
    // for QT to send packets to Arduino
    for(int i = 0; i < 2; i++){
        QByteArray datagram;
        if(state == 100)
        {
            datagram = QByteArray::number(100);
        }
        else if(state == 10)
        {
            if(ui->tableWidget->item(0,0)->isSelected()){
                ui->tableWidget->item(0,0)->setBackgroundColor(Qt::yellow);
            }
            datagram = QByteArray::number(10);
        }
        else if(state == 30)
        {
            if(ui->tableWidget->item(0,0)->isSelected()){
                ui->tableWidget->item(0,0)->setBackgroundColor(Qt::red);
            }
            datagram = QByteArray::number(30);
        }
        else if(state == 40)
        {
            if(ui->tableWidget->item(0,0)->isSelected()){
                ui->tableWidget->item(0,0)->setBackgroundColor(Qt::green);
            }
            datagram = QByteArray::number(40);
        }
        if(state == 100 || state == 10 || state == 30 || state == 40){
            socketList[i]->writeDatagram(datagram.data(), datagram.size(), QHostAddress(ip[i]), 99);
        }

        // for QT to read packets from Arduino
        if(socketList[i]->hasPendingDatagrams()) {
            datagram.resize(socketList[i]->pendingDatagramSize());
            socketList[i]->readDatagram(datagram.data(), datagram.size());

            if(QString(datagram.data()).toInt() == 40){
                ui->tableWidget->item(i,0)->setBackgroundColor(Qt::green);
                ui->tableWidget->item(i,0)->setText("House Keeping");

            }
            else if(QString(datagram.data()).toInt() == 30){
                ui->tableWidget->item(i,0)->setBackgroundColor(Qt::red);
                ui->tableWidget->item(i,0)->setText("Do Not Disturb");
            }
            else if(QString(datagram.data()).toInt() == 10){
                ui->tableWidget->item(i,0)->setBackgroundColor(Qt::yellow);
                ui->tableWidget->item(i,0)->setText("Idle");
            }
            qDebug() << "read datagram.data().toInt() = " << QString(datagram.data()).toInt();
        }
    }
}

void MainWindow::on_viewStatusButton_clicked(){
    if(checkViewMode == 1){
       if(checkStatus == 0){
           checkStatus = 1;
           checkRoom = 0;
           checkStaff = 0;
           ui->tableWidget->sortByColumn(0);
           ui->viewStatusButton->setStyleSheet("background-color: rgb(85, 255, 255)");
           ui->viewRoomButton->setStyleSheet("background-color:white");
           ui->viewStaffButton->setStyleSheet("background-color:white");
       }
       else if(checkStatus == 1){
           checkStatus = 0;
           checkRoom = 0;
           checkStaff = 0;
           state = 100;
           ui->tableWidget->sortByColumn(0);
           //ui->viewStatusButton->setStyleSheet("background-color:white");
           ui->viewRoomButton->setStyleSheet("background-color:white");
           ui->viewStaffButton->setStyleSheet("background-color:white");
       }
    }
}

void MainWindow::on_viewRoomButton_clicked(){
    if(checkViewMode == 1){
       if(checkRoom == 0){
           checkRoom = 1;
           checkStaff = 0;
           checkStatus = 0;
           ui->tableWidget->sortItems(1);
           ui->viewRoomButton->setStyleSheet("background-color: rgb(85, 255, 255)");
           ui->viewStatusButton->setStyleSheet("background-color:white");
           ui->viewStaffButton->setStyleSheet("background-color:white");
       }
       else if(checkRoom == 1){
           checkRoom = 0;
           checkStaff = 0;
           checkStatus = 0;
           state = 100;
           ui->viewStatusButton->setStyleSheet("background-color:rgb(85,255,255)");
           ui->viewRoomButton->setStyleSheet("background-color:white");
           ui->viewStaffButton->setStyleSheet("background-color:white");
       }
    }
}

void MainWindow::on_viewStaffButton_clicked(){
    if(checkViewMode == 1){
       if(checkStaff == 0){
           checkStaff = 1;
           checkRoom = 0;
           checkStatus = 0;
           ui->tableWidget->sortItems(2);
           ui->viewStaffButton->setStyleSheet("background-color: rgb(85, 255, 255)");
           ui->viewRoomButton->setStyleSheet("background-color:white");
           ui->viewStatusButton->setStyleSheet("background-color:white");
       }
       else if(checkStaff == 1){
           checkStaff = 0;
           checkRoom = 0;
           checkStatus = 0;
           state = 100;
           ui->viewStatusButton->setStyleSheet("background-color:rgb(85,255,255)");
           ui->viewRoomButton->setStyleSheet("background-color:white");
           ui->viewStaffButton->setStyleSheet("background-color:white");
       }
    }
}

void MainWindow::on_viewModeButton_clicked()
{
    ui->viewModeButton->setStyleSheet("background-color:rgb(85,255,255)");
    ui->viewStatusButton->setStyleSheet("background-color:rgb(85,255,255)");
    ui->viewRoomButton->setStyleSheet("background-color:white");
    ui->viewStaffButton->setStyleSheet("background-color:white");
    ui->editModeButton->setStyleSheet("background-color:white");
    checkStatus = 1;
    checkViewMode = 1;
    checkEditMode = 0;
}

void MainWindow::on_editModeButton_clicked()
{
    ui->viewModeButton->setStyleSheet("background-color:white");
    ui->viewStatusButton->setStyleSheet("background-color:white");
    ui->viewRoomButton->setStyleSheet("background-color:white");
    ui->viewStaffButton->setStyleSheet("background-color:white");
    ui->editModeButton->setStyleSheet("background-color:rgb(85,255,255)");
    checkStatus = 0;
    checkViewMode = 0;
    checkEditMode = 1;
}

