#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>         
#include <ezLCDLib.h>
#include <string.h>
ezLCD3 lcd;

// Each Inside LCD needs a unique mac and ip
// have 2 sets of mac and ip below to test 2 LCDs
// upload sketch with 1 or the other for each LCD
/*
byte mac[] = { 0xAA, 0xAA, 0xAA, 0xBB, 0xBB, 0xCC }; 
byte ip[] = { 172, 21, 42, 58 }; 
char *ipaddress = "172.21.42.58";
byte localPort = 99;
char room[10] = "4012";
*/
byte mac[] = { 0xAA, 0xAA, 0xAA, 0xBB, 0xBB, 0xCB };  // unique mac address
byte ip[] = { 172, 21, 42, 57 };                      // unique ip address
char *ipaddress = "172.21.42.57";                     // ip address in string form. might be used to pass into Qt
byte localPort = 99;                                  // hardcoded localPort
char room[10] = "4011";                               // room number. might be used to pass into Qt

//---------------Time Variables----------------//
IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov NTP server
const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets 


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
  printtime();
}

void printtime()
{
    //------------- get time from server ----------------//
    sendNTPpacket(timeServer); // send an NTP packet to a time server
    // wait to see if a reply is available
    delay(1000);  
    if ( Udp.parsePacket() ) {  
    // We've received a packet, read the data from it
    Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);  
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;  
    Serial.print("Seconds since Jan 1 1900 = " );
    Serial.println(secsSince1900);               

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;     
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;  
    // print Unix time:
    Serial.println(epoch);                               


    //------------ print the hour, minute and second-------------//
    //Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    //Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    //Serial.print(':');  
    char time[5];
    
    int h = (epoch % 86400) / 3600 - 7; // -7 because we're in GMT -7
    int m = (epoch  % 3600) / 60;
    
  lcd.color(WHITE);     // create color, position, angle. print out Do Not Disturb
  lcd.xy(10,90);
  lcd.fontO(1);
  lcd.println(h);
  lcd.color(WHITE);     // create color, position, angle. print out Do Not Disturb
  lcd.xy(10,65);
  lcd.fontO(1);
  lcd.println(":");
  lcd.color(WHITE);     // create color, position, angle. print out Do Not Disturb
  lcd.xy(10,60);
  lcd.fontO(1);
  lcd.println(m);
 
    
  }
  // wait ten seconds before asking for the time again
  delay(10000); 
}

unsigned long sendNTPpacket(IPAddress& address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:         
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer,NTP_PACKET_SIZE);
  Udp.endPacket(); 
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
    strcat(buffer, ipaddress);
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
