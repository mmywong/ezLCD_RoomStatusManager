#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    Room r("3024",1);
    Room r1("2021",2);
    Room r2("3456", 3);
    Room r3("6078",0);

    w.add_room(r);
    w.add_room(r1);
    w.add_room(r2);
    w.add_room(r3);
    w.show();
    return a.exec();
}
