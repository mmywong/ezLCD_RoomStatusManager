#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <String>
//#include <gridLayout>
#include <sstream>
#include <QWidget>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
   // QWidget* window = new QWidget();
    std::list<Room> mylist;

    room_number = 0;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::set_room_number(int number)
{
    room_number = number;

}

void MainWindow::add_room(Room room)
{
    std::cout << room.getID()<< std::endl;
    mylist.push_back(room);
    room_number++;

}

void MainWindow::on_pushButton_clicked()
{

   // Room* r = room_list;
    std::list<Room>::iterator it=mylist.begin();
    for (int i =0; i < room_number; i++)
    {
     //   QWidget* window = new QWidget();
      //  QGridLayout* layout = new QGridLayout;
        QLabel* l = new QLabel;
        l->setText((*it).getID());
        l->setGeometry(60, i*40 + 130, 40, 40);
     //   layout->addWidget(l,0,0);
       // window->setLayout(layout);
       // window->show();
        this->layout()->addWidget(l);



        QLabel* h = new QLabel;
        int Sta = (*it).getStatus();
        std::cout << Sta << std::endl;
        if (Sta == 0)
        {
            h->setText("DND");

        }
        else if (Sta == 1)
        {
            std::cout << "dsf" << std::endl;
            h->setText("BEDROOM");

        }
        else if (Sta == 2)
        {
            h->setText("BATHROOM");

        }
        else
        {
            h->setText("BOTH");
        }
        it++;
        h->setGeometry(600, i*40 + 130, 80, 40);
     //   layout->addWidget(l,0,0);
       // window->setLayout(layout);
       // window->show();
        this->layout()->addWidget(h);
       // ui->centralWidget-> -> addWidget(l, i, 0);

      //  this -> setLayout(layout);
       // ui -> setupUi(this);
        /* QPushButton* bottom = new QPushButton;
        std::stringstream ss;
        ss << r->getID();
        std::string str = ss.str();
        bottom -> setText("Hello");
        bottom -> setGeometry(i*40, 0, 40, 40);
        ui->centralWidget->gridLayout -> addWidget(bottom, i, 0);
        r++;
        */
      //  QLabel* label = new QLabel;
      //  QWidget* window = new QWidget();
       // QPushButton* one = new QPushButton("One",window);
      //  ui -> addWidget(label);
      //  window-> setLayout(layout);
       // window->show();

    }
}
