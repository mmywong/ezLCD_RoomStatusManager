#ifndef ROOM
#define ROOM
#include <iostream>


class Room
{
private:
    char* ID;
    int status;
public:
    Room(char* number,int s);
    ~Room();
    void setID(char* id);
    void setStatus(int s);
    char* getID();
    int getStatus();
};



#endif // ROOM

