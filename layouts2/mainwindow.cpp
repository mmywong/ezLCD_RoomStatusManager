#include "mainwindow.h"
#include "ui_mainwindow.h"

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
int MAX_ROOMS = 40;
QString ip[5] = {"172.21.42.56", "172.21.42.57"};
QString status_name(int stat_in_int);


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // intiailize configuration for tableWidget
    ui->tableWidget->setColumnCount(3); // number of columns in table
    ui->tableWidget->setRowCount(MAX_ROOMS);   // number of rows in table
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

    // ============ START of initializing random Rooms ============
    // initialize random status of rooms, room numbers, and random employee IDs

    //5 storey building with 10 rooms on each floor
    int rooms_per_floor = 10;
    int max_rooms = 40;
    Room roomlist[50];
    for(int i = 0; i < max_rooms; i++)
    {
        //generate random room info
        int roomNum = 100*(1+i/rooms_per_floor) + i%rooms_per_floor;
        int staffID = rand()%99999 + 10000; // maximum of a 5-digit random number
        int roomStatus = ((rand()%4)+1)*10;  // randomly select a number from 10/20/30/40
        if(roomStatus == 20)
            roomStatus = 10;
        int timeCleaned = 1600; // temporarily hard coded since time is not displayed for prototype
        stringstream ss;
        ss << staffID;
        roomlist[i] = Room(roomNum, roomStatus, ss.str(), timeCleaned);

        //int staffID = rand()%99999 + 10000; // maximum of a 5-digit random number
        //int roomStatus = rand()%3;  // randomly select 1 of the 3 room statuses: (Idle, Do Not Disturb, or House Keeping)

        // create tableWidget Items (cells)
        ui->tableWidget->setItem(i,0, new QTableWidgetItem(status_name(roomStatus)));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem("Room #" + QString::number(roomlist[i].getNumber())));
        ui->tableWidget->setItem(i,2, new QTableWidgetItem("Employee #" + QString::fromStdString(roomlist[i].getInCharge())));

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
        if(i>=20){
            if(ui->tableWidget->item(i%20+20,0)->text() == "Idle"){
                ui->tableWidget->item(i%20+20,0)->setBackgroundColor(Qt::yellow);
            }
            else if(ui->tableWidget->item(i%20+20,0)->text() == "Do Not Disturb"){
                ui->tableWidget->item(i%20+20,0)->setBackgroundColor(Qt::red);
            }
            else if(ui->tableWidget->item(i%20+20,0)->text() == "House Keeping"){
                ui->tableWidget->item(i%20+20,0)->setBackgroundColor(Qt::green);
            }
        }
    }

    /* initialize random status of rooms, room numbers, and random employee IDs
    for(int i = 4000; i < 4020; i++)  // randomly chose room numbers ranging from 100-119
    {
        int staffID = rand()%99999 + 10000; // maximum of a 5-digit random number
        int roomStatus = rand()%3;  // randomly select 1 of the 3 room statuses: (Idle, Do Not Disturb, or House Keeping)

        // create tableWidget Items (cells)
        ui->tableWidget->setItem(i%20+20,0, new QTableWidgetItem(status_states[roomStatus]));
        ui->tableWidget->setItem(i%20+20,1,new QTableWidgetItem("Room #" + QString::number(i)));
        ui->tableWidget->setItem(i%20+20,2, new QTableWidgetItem("Employee #" + QString::number(staffID)));

        // change the background color of the cell respective to each room status
        if(ui->tableWidget->item(i%20+20,0)->text() == "Idle"){
            ui->tableWidget->item(i%20+20,0)->setBackgroundColor(Qt::yellow);
        }
        else if(ui->tableWidget->item(i%20+20,0)->text() == "Do Not Disturb"){
            ui->tableWidget->item(i%20+20,0)->setBackgroundColor(Qt::red);
        }
        else if(ui->tableWidget->item(i%20+20,0)->text() == "House Keeping"){
            ui->tableWidget->item(i%20+20,0)->setBackgroundColor(Qt::green);
        }
    }
    */
    // ============ END of intiailizing random Rooms ============

    // filter to show information about all or specific floors
    ui->comboBox->addItem(tr("All Floors"));
    for(int i = 1; i < 5; i++)
    {
        char str[10];
        char num[10];
        itoa(i, num, 10);
        strcpy(str, "Floor ");
        strcat(str, num);
        ui->comboBox->addItem(tr(str));
    }

    // create multiple sockets
    for(int i = 0; i < 2; i++){
        socketList[i] = new QUdpSocket(this);
    }

    // connect the timer to the comm() function to constantly read packet data sent from Arduinos to Qt
    connect(&timer, SIGNAL(timeout()), this, SLOT(communicate_Qt_Arduino()));
    timer.start(100);

    // connect view mode and edit mode buttons to a single function
    connect(ui->viewModeButton, SIGNAL(clicked()), this, SLOT(ModeButton_clicked()));
    connect(ui->editModeButton, SIGNAL(clicked()), this, SLOT(ModeButton_clicked()));

    // connect different sorted view states to a single function
    connect(ui->viewStatusButton, SIGNAL(clicked()), this, SLOT(changeViewOption()));
    connect(ui->viewRoomButton, SIGNAL(clicked()), this, SLOT(changeViewOption()));
    connect(ui->viewStaffButton, SIGNAL(clicked()), this, SLOT(changeViewOption()));

    // connect edit mode's change to idle, change to do not disturb and change to house keeping buttons to a single function
    connect(ui->changeStatusIdle, SIGNAL(clicked()), this, SLOT(changeStatusOption()));
    connect(ui->changeStatusDND, SIGNAL(clicked()), this, SLOT(changeStatusOption()));
    connect(ui->changeStatusHK, SIGNAL(clicked()), this, SLOT(changeStatusOption()));


}


MainWindow::~MainWindow()
{
    delete ui;
}

QString status_name(int stat_in_int){
    if(stat_in_int == 10)
        return IDLE;
    //if(stat_in_int == 20)
      //  return IDLE;
    if(stat_in_int == 30)
        return DND;
    if(stat_in_int == 40)
        return HK;
}

void MainWindow::changeStatusOption()
{
    QPushButton *b = qobject_cast<QPushButton *>(sender());
    if(checkEditMode == 1)
    {
        if(b == ui->changeStatusIdle)
        {
            state = 10;
            ui->changeStatusIdle->setStyleSheet("background-color:yellow");
            ui->changeStatusDND->setStyleSheet("background-color:white");
            ui->changeStatusHK->setStyleSheet("background-color:white");

            ui->tableWidget->setCurrentItem(ui->tableWidget->item(0,0));
            if(ui->tableWidget->currentItem()->isSelected())
            {
                ui->tableWidget->currentItem()->setBackgroundColor(Qt::yellow);
            }
        }
        else if(b == ui->changeStatusDND)
        {
            state = 30;
            if(selectDND == 0)
            {
                selectDND = 1;
                ui->changeStatusDND->setStyleSheet("background-color:red");
                ui->changeStatusIdle->setStyleSheet("background-color:white");
            }
            else if(selectDND == 1)
            {
                selectDND = 0;
                ui->changeStatusDND->setStyleSheet("background-color:white");
                ui->changeStatusIdle->setStyleSheet("background-color:yellow");
            }
            selectHK = 0;
            ui->changeStatusHK->setStyleSheet("background-color:white");

            ui->tableWidget->setCurrentItem(ui->tableWidget->item(0,0));
            if(ui->tableWidget->currentItem()->isSelected())
            {
                ui->tableWidget->currentItem()->setBackgroundColor(Qt::red);
            }
        }
        else if(b == ui->changeStatusHK)
        {
            state = 40;
            if(selectHK == 0)
            {
                selectHK = 1;
                ui->changeStatusHK->setStyleSheet("background-color:green");
                ui->changeStatusIdle->setStyleSheet("background-color:white");
            }
            else if(selectHK == 1)
            {
                selectHK = 0;
                ui->changeStatusHK->setStyleSheet("background-color:white");
                ui->changeStatusIdle->setStyleSheet("background-color:yellow");
            }

            selectDND = 0;
            ui->changeStatusDND->setStyleSheet("background-color:white");

            ui->tableWidget->setCurrentItem(ui->tableWidget->item(0,0));
            if(ui->tableWidget->currentItem()->isSelected())
            {
                ui->tableWidget->currentItem()->setBackgroundColor(Qt::green);
            }
        }
    }
}

void MainWindow::communicate_Qt_Arduino()
{
    // for QT to send packets to Arduino
    QByteArray datagram;
    for(int i = 0; i < 2; i++){
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
    }
    // for QT to read packets from Arduino
    for(int i = 0; i < 2; i++)
    {
        if(socketList[i]->hasPendingDatagrams())
        {
            datagram.resize(socketList[i]->pendingDatagramSize());
            socketList[i]->readDatagram(datagram.data(), datagram.size());

            if(QString(datagram.data()).toInt() == 40)
            {
                ui->tableWidget->item(i,0)->setBackgroundColor(Qt::green);
                ui->tableWidget->item(i,0)->setText("House Keeping");
            }
            else if(QString(datagram.data()).toInt() == 30)
            {
                ui->tableWidget->item(i,0)->setBackgroundColor(Qt::red);
                ui->tableWidget->item(i,0)->setText("Do Not Disturb");
            }
            else if(QString(datagram.data()).toInt() == 10)
            {
                ui->tableWidget->item(i,0)->setBackgroundColor(Qt::yellow);
                ui->tableWidget->item(i,0)->setText("Idle");
            }
            // prints out state, room#, ipaddress from Arduino
            qDebug() << datagram.data();
        }
    }
}

void MainWindow::changeViewOption()
{
     QPushButton *b = qobject_cast<QPushButton *>(sender());
     if(checkViewMode == 1)
     {
         if(b == ui->viewStatusButton)
         {
            if(checkStatus == 0)
            {
                checkStatus = 1;
                ui->viewStatusButton->setStyleSheet("background-color: rgb(85, 255, 255)");
            }
            else if(checkStatus == 1)
            {
                checkStatus = 0;
                state = 100;
            }
            checkRoom = 0;
            checkStaff = 0;
            ui->tableWidget->sortByColumn(0);
            ui->viewRoomButton->setStyleSheet("background-color:white");
            ui->viewStaffButton->setStyleSheet("background-color:white");

         }
         else if(b == ui->viewRoomButton)
         {
            if(checkRoom == 0)
            {
                checkRoom = 1;
                ui->tableWidget->sortItems(1);
                ui->viewRoomButton->setStyleSheet("background-color: rgb(85, 255, 255)");
                ui->viewStatusButton->setStyleSheet("background-color:white");
            }
            else if(checkRoom == 1)
            {
                checkRoom = 0;
                state = 100;
                ui->viewStatusButton->setStyleSheet("background-color:rgb(85,255,255)");
                ui->viewRoomButton->setStyleSheet("background-color:white");
            }
            checkStaff = 0;
            checkStatus = 0;
            ui->viewStaffButton->setStyleSheet("background-color:white");
         }
         else if(b == ui->viewStaffButton)
         {
            if(checkStaff == 0)
            {
                checkStaff = 1;
                ui->tableWidget->sortItems(2);
                ui->viewStaffButton->setStyleSheet("background-color: rgb(85, 255, 255)");
                ui->viewStatusButton->setStyleSheet("background-color:white");
            }
            else if(checkStaff == 1)
            {
                checkStaff = 0;
                state = 100;
                ui->viewStatusButton->setStyleSheet("background-color:rgb(85,255,255)");
                ui->viewStaffButton->setStyleSheet("background-color:white");
            }
            checkRoom = 0;
            checkStatus = 0;
            ui->viewRoomButton->setStyleSheet("background-color:white");
         }
     }
}

void MainWindow::ModeButton_clicked()
{
    QPushButton *b = qobject_cast<QPushButton *>(sender());
    if(b == ui->viewModeButton)
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
    else if(b == ui->editModeButton)
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
}

// tool button to filter displayed info by floor
// still need to make it dynamic and filter out the correct room numbers by row
// ex. floor 3 will reveal all rows with room numbers starting with 3###
void MainWindow::on_comboBox_activated(const QString &arg1)
{
    if(arg1 == "All Floors")
    {
        for(int i = 0; i < MAX_ROOMS; i++)
        {
            ui->tableWidget->showRow(i);
        }
    }
    else if(arg1 == "Floor 1")
    {
        for(int i = 0; i < MAX_ROOMS; i++)
        {
            ui->tableWidget->showRow(i);
        }
        ui->tableWidget->show();
        for(int i = 0; i < ui->tableWidget->rowCount(); i++)
        {
            QTableWidgetItem *item = ui->tableWidget->item(i, 1);
            if(!item->text().contains("Room #10"))
            {
                ui->tableWidget->hideRow(i);
            }
        }
    }
    else if(arg1 == "Floor 2")
    {
        for(int i = 0; i < MAX_ROOMS; i++)
        {
            ui->tableWidget->showRow(i);
        }
        ui->tableWidget->show();
        for(int i = 0; i < ui->tableWidget->rowCount(); i++)
        {
            QTableWidgetItem *item = ui->tableWidget->item(i, 1);
            if(!item->text().contains("Room #20"))
            {
                ui->tableWidget->hideRow(i);
            }
        }
    }
    else if(arg1 == "Floor 3")
    {
        for(int i = 0; i < MAX_ROOMS; i++)
        {
            ui->tableWidget->showRow(i);
        }
        ui->tableWidget->show();
        for(int i = 0; i < ui->tableWidget->rowCount(); i++)
        {
            QTableWidgetItem *item = ui->tableWidget->item(i, 1);
            if(!item->text().contains("Room #30"))
            {
                ui->tableWidget->hideRow(i);
            }
        }
    }
    else if(arg1 == "Floor 4")
    {
        for(int i = 0; i < MAX_ROOMS; i++)
        {
            ui->tableWidget->showRow(i);
        }
        ui->tableWidget->show();
        for(int i = 0; i < ui->tableWidget->rowCount(); i++)
        {
            QTableWidgetItem *item = ui->tableWidget->item(i, 1);
            if(!item->text().contains("Room #40"))
            {
                ui->tableWidget->hideRow(i);
            }
        }
    }
}
