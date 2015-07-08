#include "room.h"

Room::Room(char* number, int s)
{
    ID = number, status = s;
}

Room::~Room()
{
    ID = 0;
    status = 0;

}

char* Room::getID()
{

    return ID;
}

int Room::getStatus()
{
    return status;
}

void Room::setID(char* id)
{
    ID = id;

}

void Room::setStatus(int s)
{
    status = s;

}
