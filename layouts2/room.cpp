#include "room.h"

Room::Room()
{
    number = 0;
    status = 0;
    incharge = "";
    time = -1;
    ipaddress = "";
}

Room::Room(int new_number, int new_status, string new_incharge, string new_time, QString new_ipaddress)
{
    number = new_number;
    status = new_status;
    incharge = new_incharge;
    time = new_time;
    ipaddress = new_ipaddress;
}

int Room::getNumber() const
{
    return number;
}

int Room::setNumber(int num)
{
    number = num;
    return number;
}

int Room::getStatus()
{
    return status;
}

void Room::setStatus(int value)
{
    status = value;
}

string Room::getInCharge()
{
    return incharge;
}

void Room::setInCharge(string staff)
{
    incharge = staff;
}

string Room::getTime()
{
    return time;
}

void Room::setTime(int value)
{
    char temp[5] = "";
    itoa(value, temp, 10);
    time = temp;
}

QString Room::getIP()
{
    return ipaddress;
}

void Room::setIP(QString ipaddr)
{
    ipaddress = ipaddr;
}
