#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>         
#include <ezLCDLib.h>
#include <string.h>
ezLCD3 lcd;

// Each Inside LCD needs a unique mac and ip
// have 2 sets of mac and ip below to test 2 LCDs
// upload sketch with 1 or the other for each LCD

byte mac[] = { 0xAA, 0xAA, 0xAA, 0xBB, 0xBB, 0xCC }; 
byte ip[] = { 172, 21, 42, 58 }; 
char *ipaddress = "172.21.42.58";
byte localPort = 99;
char room[10] = "4012";
/*
byte mac[] = { 0xAA, 0xAA, 0xAA, 0xBB, 0xBB, 0xCB };  // unique mac address
byte ip[] = { 172, 21, 42, 57 };                      // unique ip address
char *ipaddress = "172.21.42.57";                     // ip address in string form. might be used to pass into Qt
byte localPort = 99;                                  // hardcoded localPort
char room[10] = "4011";                               // room number. might be used to pass into Qt
*/
char roombuf[10]; // buffer to hold the room number

EthernetUDP Udp;
static char buffer[UDP_TX_PACKET_MAX_SIZE]; 

// states
enum States{Initial, HomeScreen, Select, DND, HK} state;
int stateIDLE = 10;
int stateDND = 30;
int stateHK = 40;

//Changing states based on reading from QT
int st = 0;
int r = 0;  // r is read value from Qt for any state changes

void Tick_State(){
  switch(state){
    case Initial: //the first state. set to choice.
      Choice();
      state = Select;
      break;
    case Select:  // while choice, check inputs, switch input to home, DND, or HK. become state.
      if((lcd.currentWidget==3) && (lcd.currentInfo==PRESSED) || r == stateDND)
      {                                                           //widget 3 put over do not disturb is a touchzone.
        DoNotDisturb();                                           //being pressed take you to the do not disturb screen
        state = DND;                                              //state changed to DND
      }
      else if((lcd.currentWidget==4) && (lcd.currentInfo==PRESSED) || r == stateHK)
      {                                                           // widget 4 put over house keeping is a touchzone
        HouseKeeping();                                           // being pressed takes you to the house keeping screen
        state = HK;                                               // state changed to HK
      }
      break;
    case DND: // same as homescreen.
      if((lcd.currentWidget==1) && (lcd.currentInfo==PRESSED) || r == stateIDLE)
      {                                                         // widget 1 is on DND and HK screen
        Choice();                                               // being pressed takes you to the Choice screen
        state = Select;                                         // state changed to Select
      }
      else if(r == stateHK)
      {
        HouseKeeping();                                           // being pressed takes you to the house keeping screen
        state = HK;
      }
      break;
    case HK: // same as homescreen
      if((lcd.currentWidget==1) && (lcd.currentInfo==PRESSED) || r == stateIDLE)
      {                                                        // widget 1 is on DND and HK screen
        Choice();                                              // being pressed takes you to the Choice screen
        state = Select;                                        // state changed to Select
      }
      else if(r == stateDND)
      {
        DoNotDisturb();                                           //being pressed take you to the do not disturb screen
        state = DND;
      }
      break;
    default: // default state
      state = Initial;
      break;
  }

  switch(state)
  {
    case Select:
      //Choice();
      st = stateIDLE;
      break;
    case DND:
      //DoNotDisturb();
      st = stateDND;
      break;
    case HK:
      //HouseKeeping();
      st = stateHK;
      break;
    default:
      break;
  }
}

void setup()
{
  lcd.begin( EZM_BAUD_RATE );
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);
  strcpy(roombuf, room);
  state = Initial; // first state becomes choice();
}

void loop()
{
  lcd.wstack(0);
  Tick_State(); // state machine activation
  writeStatus(st);
}

// send status update from LCD to Qt
void writeStatus(int x){
  int packetSize = Udp.parsePacket();
  if(packetSize)
  {
    static char buffer[UDP_TX_PACKET_MAX_SIZE];

    // clear buffer
    for(int i = 0; i < UDP_TX_PACKET_MAX_SIZE; ++i)
    {
      buffer[i] = 0; 
    }
    Udp.read(buffer,UDP_TX_PACKET_MAX_SIZE);  // read new data from Qt and store into buffer
    
    r = atoi(buffer);
    // if read data from Qt is dummy data 100 and is not the curren status on LCD, new state is read data from Qt
    if(r != 100  && r != x)
    {
      x = r;
    }   
    itoa(x, buffer, 10);

    strcat(buffer, roombuf);
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(buffer);
    Udp.endPacket();

    /*
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(buffer);
    Udp.endPacket();

    strcpy(buffer, room);
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(buffer);
    Udp.endPacket();

    strcpy(buffer, ipaddress);
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(buffer);
    Udp.endPacket();
    */
  }
}

void Choice() //choice function. multiple end points using touchzones.
{
  lcd.cls(BLACK);
  
  lcd.light(0);                 //light off. pring image. light on.
  lcd.picture("Metro0.bmp"); 
  lcd.light(100);

  lcd.font("Serif24");    // create font, color, position, angle. print out House Keeping
  lcd.color(WHITE);
  lcd.xy(165,155);
  lcd.fontO(1);
  lcd.print("House");
  lcd.color(WHITE);
  lcd.xy(185,155);
  lcd.fontO(1);
  lcd.print("Keeping");
 
  lcd.color(WHITE);     // create color, position, angle. print out Do Not Disturb
  lcd.xy(110,155);
  lcd.fontO(1);
  lcd.print("Do Not");
  lcd.color(WHITE);
  lcd.xy(130,155);
  lcd.fontO(1);
  lcd.print("Disturb");

  lcd.font("0");
  lcd.fontO(1);
  lcd.color(WHITE);
  lcd.xy(235,155);
  lcd.println("Touch To");
  lcd.xy(250,200);
  lcd.println("Change Room Status");
  
  lcd.touchZone(3,110,35,50,170,1);  // touchzone 3 takes you to state DND
  lcd.touchZone(4,165,35,50,170,1);  // touchzone 4 takes you to state HK
}

void HouseKeeping() //housekeeping. loop to choice on touchznoe.
{
  lcd.light(0);        // light off, ptint picture position, light on
  lcd.picture(100,60,"clean.bmp");
  lcd.light(100);
 
  lcd.touchZone(1,0,0,320,240,1); //create touchzone for looping to choice
}

void DoNotDisturb() // do not disturb. loop to choice on touchzone.
{
  lcd.light(0);       // light off, ptint picture position, light on
  lcd.picture(100,60,"dont.bmp");
  lcd.light(100);
  
  lcd.touchZone(1,0,0,320,240,1); //create touchzone for looping to choice
}
