#include "room.h"

Room::Room()
{
    number = 0;
    status = 0;
    incharge = "";
    time = -1;
}

Room::Room(int new_number, int new_status, string new_incharge, int new_time)
{
    number = new_number;
    status = new_status;
    incharge = new_incharge;
    time = new_time;
}

int Room::getNumber() const
{
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

int Room::getTime()
{
    return time;
}

void Room::setTime(int value)
{
    time = value;
}
