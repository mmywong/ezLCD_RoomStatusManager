#ifndef ROOM_H
#define ROOM_H
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

using namespace std;

class Room
{
public:

    Room();
    Room(int new_number, int new_status, string new_incharge, string new_time, QString new_ipaddress);

    //-----getter and setter-----//
    int getNumber() const;
    int setNumber(int num);
    int getStatus();
    void setStatus(int value);

    string getInCharge();
    void setInCharge(string staff);

    string getTime();
    void setTime(int value);

    QString getIP();
    void setIP(QString ipaddr);

private:
    int number;
    int status; //10: idle, 30: do not disturb, 40: housekeeping, 100: read current state
    string incharge;
    string time; //4 digit number representing time cleaned. e.g. 1600. time = -1 if room not cleaned.
    QString ipaddress;

};

#endif // ROOM_H
