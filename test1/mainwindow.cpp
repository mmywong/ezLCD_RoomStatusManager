#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_StatusButton_clicked()
{
    /*
    QPushButton *room = new QPushButton;
    room->setText("313");
    room->setMaximumWidth(50);
    QPushButton *room2 = new QPushButton;
    room2->setText("512");
    room2->setMaximumWidth(50);

    ui->statuslayout->layout()->addWidget(room);
    ui->statuslayout->layout()->addWidget(room2);
    */

    QPushButton *rooms[1][1];
    for(int i = 0; i < 1; i++)
    {
        for(int j = 0; j < 1; j++)
        {
            rooms[i][j] = new QPushButton();
            rooms[i][j]->setText("393");
            rooms[i][j]->setGeometry(j*40, i*40, 40,40 ); //qrect
            ui->gridLayout->addWidget(rooms[i][j], i, j);
            //ui->gridLayout->layout()->addWidget(rooms[i][j], i, j);

        }
    }

    //ui->statusrooms->setLayout(ui->gridLayout);
    //setCentralWidget(ui->statusrooms);
    //centralWidget->setLayout(ui->gridLayout);
    //setCentralWidget(centralWidget);

}
