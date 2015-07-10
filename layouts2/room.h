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
    int status; //10: idle, 20: select screen, 30: do not disturb, 40: housekeeping, 100: read current state
    string incharge;
    int time; //4 digit number representing time cleaned. e.g. 1600. time = -1 if room not cleaned.

};

#endif // ROOM_H
