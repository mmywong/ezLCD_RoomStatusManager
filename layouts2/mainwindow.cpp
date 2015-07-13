#include "mainwindow.h"
#include "ui_mainwindow.h"

// determines 1 of 2 modes either to view/sort the information OR to edit the status of the rooms
int checkViewMode = 1;
int checkEditMode = 0;

// determines the room status in the form of an int to send to/retrieve from the Arduino
int state = 100;
int stateIdle = 10;
int stateDND = 30;
int stateHK = 40;
int stateCurrent = 100;


// determines if Status/Staff/Room buttons have been pressed
int checkStatus = 1;
int checkStaff = 0;
int checkRoom = 0;

// determine if the Idle/DND/HK buttons have been pressed
int selectIdle = 0;
int selectDND = 0;
int selectHK = 0;

// change text display here
QString textIDLE = "Idle";
QString textDND = "Do Not Disturb";
QString textHK = "House Keeping";
QString status_states[3] = {textIDLE, textDND, textHK};
const char* WHITE = "background-color:white";
const char* RED = "background-color:red";
const char* GREEN = "background-color:green";
const char* LIGHT_BLUE = "background-color:rgb(85,255,255)";
const char* YELLOW = "background-color:yellow";

// array of used IP addresses by Arduinos
QString ip[5] = {"172.21.42.57", "172.21.42.58"};

//
const int max_rooms = 40;
int rooms_per_floor = 15;
Room roomlist[max_rooms];
QString status_name(int stat_in_int);

// initial Main Window setup
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // intiailize configuration for tableWidget
    ui->tableWidget->setColumnCount(3); // number of columns in table
    ui->tableWidget->setRowCount(max_rooms);   // number of rows in table
    ui->tableWidget->setShowGrid(false);    // hide the grid lines
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);    // disable editing for table cells
    ui->tableWidget->horizontalHeader()->setVisible(false); // hide the horizontal headers at the top of the table
    ui->tableWidget->setSelectionMode(QTableWidget::NoSelection);   // hide the highlighting of a cell when a cell is selected

    // resize the columns to evenly spread out along the whole size of the table
    for (int c = 0; c < ui->tableWidget->horizontalHeader()->count(); ++c)  // horizontalHeader()->count() is the total number of headers/columns
    {
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(c, QHeaderView::Stretch);
    }

    // ============ START of initializing random Rooms ============
    // initialize random status of rooms, room numbers, and random employee IDs

    srand(time(NULL));  // for generating random employee ID number

    //4 storey building with 10 rooms on each floor
    for(int i = 0; i < max_rooms; i++)
    {
        //generate random room info
        int roomNum = 1000*(1+i/rooms_per_floor) + i%rooms_per_floor;
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
        if(ui->tableWidget->item(i%max_rooms,0)->text() == textIDLE){
            ui->tableWidget->item(i%max_rooms,0)->setBackgroundColor(Qt::yellow);
        }
        else if(ui->tableWidget->item(i%max_rooms,0)->text() == textDND){
            ui->tableWidget->item(i%max_rooms,0)->setBackgroundColor(Qt::red);
        }
        else if(ui->tableWidget->item(i%max_rooms,0)->text() == textHK){
            ui->tableWidget->item(i%max_rooms,0)->setBackgroundColor(Qt::green);
        }
    }

    // ============ END of intiailizing random Rooms ============

    // initialize combobox widget for filter to show information about all or specific floors
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

    // create multiple sockets to send/retrieve data packets to/from Arduino
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

// deconstructor
MainWindow::~MainWindow()
{
    delete ui;
}

// sets the text status for a room
QString status_name(int stat_in_int){
    if(stat_in_int == stateIdle)
        return textIDLE;
    //if(stat_in_int == 20)
      //  return IDLE;
    if(stat_in_int == stateDND)
        return textDND;
    if(stat_in_int == stateHK)
        return textHK;
}

// function that controls button events for Edit Mode's Idle/DND/HK buttons and changes room's statuses by selecting a cell
void MainWindow::changeStatusOption()
{
    QPushButton *b = qobject_cast<QPushButton *>(sender());
    if(checkEditMode == 1)
    {
        ui->tableWidget->setSelectionMode(QTableWidget::SingleSelection);
        int col = ui->tableWidget->currentColumn() + 1;
        int row = ui->tableWidget->currentRow();
        QString room = ui->tableWidget->item(row,col)->text();
        room = room.remove("Room #");
        if(ui->tableWidget->selectedItems().size() != 0)
        {
            QTableWidgetItem *item = ui->tableWidget->currentItem();
            if(b == ui->changeStatusIdle)
            {
                state = stateIdle;
                ui->changeStatusIdle->setStyleSheet(YELLOW);
                ui->changeStatusDND->setStyleSheet(WHITE);
                ui->changeStatusHK->setStyleSheet(WHITE);

                if(item->isSelected())
                {
                    if(item->column() == 0)
                    {
                        item->setBackgroundColor(Qt::yellow);
                        item->setText(textIDLE);

                        //change room status
                        for(int i = 0; i<max_rooms; i++)
                        {
                            if(roomlist[i].getNumber() == room.toInt())
                            {
                                roomlist[i].setStatus(stateIdle);
                                qDebug() << "Room #" << roomlist[i].getNumber() << "|Status: " << roomlist[i].getStatus();
                            }
                        }
                    }
                }
            }
            else if(b == ui->changeStatusDND)
            {
                state = stateDND;
                if(selectDND == 0)
                {
                    selectDND = 1;
                    ui->changeStatusDND->setStyleSheet(RED);
                    ui->changeStatusIdle->setStyleSheet(WHITE);
                }
                else if(selectDND == 1)
                {
                    selectDND = 0;
                    ui->changeStatusDND->setStyleSheet(WHITE);
                    ui->changeStatusIdle->setStyleSheet(YELLOW);
                }
                selectHK = 0;
                ui->changeStatusHK->setStyleSheet(WHITE);

                if(item->isSelected())
                {
                    if(item->column() == 0)
                    {
                        ui->tableWidget->currentItem()->setBackgroundColor(Qt::red);
                        ui->tableWidget->currentItem()->setText(textDND);

                        //change room status
                        for(int i = 0; i<max_rooms; i++)
                        {
                            if(roomlist[i].getNumber() == room.toInt())
                            {
                                roomlist[i].setStatus(stateDND);
                                qDebug() << "Room #" << roomlist[i].getNumber() << "|Status: " << roomlist[i].getStatus();
                            }
                        }
                    }
                }
            }
            else if(b == ui->changeStatusHK)
            {
                state = stateHK;
                if(selectHK == 0)
                {
                    selectHK = 1;
                    ui->changeStatusHK->setStyleSheet(GREEN);
                    ui->changeStatusIdle->setStyleSheet(WHITE);
                }
                else if(selectHK == 1)
                {
                    selectHK = 0;
                    ui->changeStatusHK->setStyleSheet(WHITE);
                    ui->changeStatusIdle->setStyleSheet(YELLOW);
                }

                selectDND = 0;
                ui->changeStatusDND->setStyleSheet(WHITE);

                if(item->isSelected())
                {
                    if(item->column() == 0)
                    {
                        item->setBackgroundColor(Qt::green);
                        item->setText(textHK);

                        //change room status
                        for(int i = 0; i<max_rooms; i++)
                        {
                            if(roomlist[i].getNumber() == room.toInt())
                            {
                                roomlist[i].setStatus(stateHK);
                                qDebug() << "Room #" << roomlist[i].getNumber() << "|Status: " << roomlist[i].getStatus();
                            }
                        }
                    }
                }
            }
        } 
    }
}

// function to send/retrieve data packets to/from Arduinos
void MainWindow::communicate_Qt_Arduino()
{
    // for QT to send packets to Arduino
    QByteArray datagram;
    ui->tableWidget->setSelectionMode(QTableWidget::SingleSelection);
    QTableWidgetItem *item = ui->tableWidget->currentItem();

    for(int i = 0; i < 2; i++)
    {
        if(state == stateCurrent)
        {
            datagram = QByteArray::number(stateCurrent);
        }
        else if(state == stateIdle)
        {
            datagram = QByteArray::number(stateIdle);
        }
        else if(state == stateDND)
        {
            datagram = QByteArray::number(stateDND);
        }
        else if(state == stateHK)
        {
            datagram = QByteArray::number(stateHK);
        }
        if(state == stateIdle || state == stateDND || state == stateHK)
        {
            if(ui->tableWidget->selectedItems().size() != 0)
            {
                if(item->isSelected())
                {
                    if(item->column() == 0)
                    {

                        socketList[i]->writeDatagram(datagram.data(), datagram.size(), QHostAddress(ip[i]), 99);    // establishes socket binding connection between Qt and Arduino
                    }
                }
            }
        }
        else if(state == stateCurrent)
        {
             socketList[i]->writeDatagram(datagram.data(), datagram.size(), QHostAddress(ip[i]), 99);    // establishes socket binding connection between Qt and Arduin
        }
    }
    // for QT to read packets from Arduino
    for(int i = 0; i < 2; i++)
    {
        if(socketList[i]->hasPendingDatagrams())
        {
            datagram.resize(socketList[i]->pendingDatagramSize());
            socketList[i]->readDatagram(datagram.data(), datagram.size());

            if(QString(datagram.data()).toInt() == stateIdle)
            {
                ui->tableWidget->item(i,0)->setBackgroundColor(Qt::yellow);
                ui->tableWidget->item(i,0)->setText(textIDLE);
            }
            else if(QString(datagram.data()).toInt() == stateDND)
            {
                ui->tableWidget->item(i,0)->setBackgroundColor(Qt::red);
                ui->tableWidget->item(i,0)->setText(textDND);
            }
            else if(QString(datagram.data()).toInt() == stateHK)
            {
                ui->tableWidget->item(i,0)->setBackgroundColor(Qt::green);
                ui->tableWidget->item(i,0)->setText(textHK);
            }
            // prints out state, room#, ipaddress from Arduino
            //qDebug() << datagram.data();
        }
    }
}

// function to determine if View Mode's Status/Room/Staff buttons have been pressed
void MainWindow::changeViewOption()
{
     QPushButton *b = qobject_cast<QPushButton *>(sender());
     if(checkViewMode == 1)
     {
         ui->tableWidget->setSelectionMode(QTableWidget::NoSelection);
         if(b == ui->viewStatusButton)
         {
            if(checkStatus == 0)
            {
                checkStatus = 1;
                ui->viewStatusButton->setStyleSheet(LIGHT_BLUE);
            }
            else if(checkStatus == 1)
            {
                checkStatus = 0;
                state = stateCurrent;
            }
            checkRoom = 0;
            checkStaff = 0;
            ui->tableWidget->sortByColumn(0);
            ui->viewRoomButton->setStyleSheet(WHITE);
            ui->viewStaffButton->setStyleSheet(WHITE);

         }
         else if(b == ui->viewRoomButton)
         {
            if(checkRoom == 0)
            {
                checkRoom = 1;
                ui->tableWidget->sortItems(1);
                ui->viewRoomButton->setStyleSheet(LIGHT_BLUE);
                ui->viewStatusButton->setStyleSheet(WHITE);
            }
            else if(checkRoom == 1)
            {
                checkRoom = 0;
                state = stateCurrent;
                ui->viewStatusButton->setStyleSheet(LIGHT_BLUE);
                ui->viewRoomButton->setStyleSheet(WHITE);
            }
            checkStaff = 0;
            checkStatus = 0;
            ui->viewStaffButton->setStyleSheet(WHITE);
         }
         else if(b == ui->viewStaffButton)
         {
            if(checkStaff == 0)
            {
                checkStaff = 1;
                ui->tableWidget->sortItems(2);
                ui->viewStaffButton->setStyleSheet(LIGHT_BLUE);
                ui->viewStatusButton->setStyleSheet(WHITE);
            }
            else if(checkStaff == 1)
            {
                checkStaff = 0;
                state = stateCurrent;
                ui->viewStatusButton->setStyleSheet(LIGHT_BLUE);
                ui->viewStaffButton->setStyleSheet(WHITE);
            }
            checkRoom = 0;
            checkStatus = 0;
            ui->viewRoomButton->setStyleSheet(WHITE);
         }
     }
}

void MainWindow::ModeButton_clicked()
{
    QPushButton *b = qobject_cast<QPushButton *>(sender());
    if(b == ui->viewModeButton)
    {
        ui->viewModeButton->setStyleSheet(LIGHT_BLUE);
        ui->editModeButton->setStyleSheet(WHITE);
        ui->changeStatusIdle->setStyleSheet(WHITE);
        ui->changeStatusDND->setStyleSheet(WHITE);
        ui->changeStatusHK->setStyleSheet(WHITE);
        checkStatus = 1;
        checkViewMode = 1;
        checkEditMode = 0;
    }
    else if(b == ui->editModeButton)
    {
        ui->viewModeButton->setStyleSheet(WHITE);
        ui->viewStatusButton->setStyleSheet(WHITE);
        ui->editModeButton->setStyleSheet(LIGHT_BLUE);
        checkStatus = 0;
        checkViewMode = 0;
        checkEditMode = 1;
    }
    ui->viewRoomButton->setStyleSheet(WHITE);
    ui->viewStaffButton->setStyleSheet(WHITE);
}

// tool button to filter displayed info by floor
// still need to make it dynamic and filter out the correct room numbers by row
// ex. floor 3 will reveal all rows with room numbers starting with 3###
void MainWindow::on_comboBox_activated(const QString &arg1)
{
    // show all rows
    for(int i = 0; i < max_rooms; i++)
    {
        ui->tableWidget->showRow(i);
    }
    ui->tableWidget->show();


    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
         QTableWidgetItem *item = ui->tableWidget->item(i, 1);
         if(arg1 == "All Floors")
         {
            // do nothing. All rows shown in previous loop
         }
         else if(arg1 == "Floor 1")
         {
             if(!item->text().contains("Room #10"))
             {
                 ui->tableWidget->hideRow(i);
             }
         }
         else if(arg1 == "Floor 2")
         {
             if(!item->text().contains("Room #20"))
             {
                 ui->tableWidget->hideRow(i);
             }
         }
         else if(arg1 == "Floor 3")
         {
             if(!item->text().contains("Room #30"))
             {
                 ui->tableWidget->hideRow(i);
             }
         }
         else if(arg1 == "Floor 4")
         {
             if(!item->text().contains("Room #40"))
             {
                 ui->tableWidget->hideRow(i);
             }
         }
    }
}
