#ifndef ROOM_H
#define ROOM_H
#include <iostream>

using namespace std;

class Room
{
public:

    Room();
    Room(int new_number, int new_status, string new_incharge, int new_time);

    //-----getter and setter-----//
    int getNumber() const;

    int getStatus();
    void setStatus(int value);

    string getInCharge();
    void setInCharge(string staff);

    int getTime();
    void setTime(int value);

private:
    int number;
    int status; //0: not clean, 1: clean, 2: only bedroom clean, 3: only bathroom clean
    string incharge;
    int time; //4 digit number representing time cleaned. e.g. 1600. time = -1 if room not cleaned.

};

#endif // ROOM_H
