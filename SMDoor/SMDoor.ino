#include <ezLCDLib.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>         
#include <ezLCDLib.h>
#include <string.h>

ezLCD3 lcd;
byte mac[] = { 0xAA, 0xAA, 0xAA, 0xBB, 0xBB, 0xCC }; 
byte ip[] = { 172, 21, 42, 58 };    
char *ipaddress = "172.21.42.58";
byte localPort = 99;
char room[10] = "4012";
char roombuf[10];

EthernetUDP Udp;
static char buffer2[UDP_TX_PACKET_MAX_SIZE];

//Changing states based on reading from QT
int st = 0;
int r = 0;

enum States{Initial, Home, Select, DND, HK} state;

int stateIDLE = 10;
int stateDND = 30;
int stateHK = 40;

//State Machine function for the Hotel Door
//Draws on Transitions, writes status to QT as state actions
void Tick_State(){
  switch(state){
    case Initial:
      HomeM();
      state = Home;
      break;
    case Home:
      if((lcd.currentWidget==1) && (lcd.currentInfo==PRESSED)){
          Choice();
          state = Select;
      }
      else if(!((lcd.currentWidget==1) && (lcd.currentInfo==PRESSED))){
          state = Home;
      }
      break;
    case Select:
      if(((lcd.currentWidget==2) && (lcd.currentInfo==PRESSED)) || r == stateIDLE){
           HomeM();
           state = Home;
      }else if(((lcd.currentWidget==3) && (lcd.currentInfo==PRESSED)) || r == stateDND){
           HomeD();
           state = DND;
      }else if(((lcd.currentWidget==4) && (lcd.currentInfo==PRESSED)) || r == stateHK){
           HomeH();
           state = HK;
      }
      else if(!(((lcd.currentWidget==2) && (lcd.currentInfo==PRESSED)) || ((lcd.currentWidget==3) && (lcd.currentInfo==PRESSED)) || ((lcd.currentWidget==4) && (lcd.currentInfo==PRESSED)))){
           state = Select;
      }
      break;
    case DND:
      if((lcd.currentWidget==1) && (lcd.currentInfo==PRESSED)){ 
          Choice();
          state = Select;
      }
      else if(!((lcd.currentWidget==1) && (lcd.currentInfo==PRESSED))){
          state = DND;
      }
      break;
    case HK:
      if((lcd.currentWidget==1) && (lcd.currentInfo==PRESSED)){
          Choice();
          state = Select;
      }
      else if(!((lcd.currentWidget==1) && (lcd.currentInfo==PRESSED))){
          state = HK;
      }
      break;
    default:
      state = Initial;
      break;
  }
  
  switch(state){
    case Home:
      st = stateIDLE;
      break;
    case Select:
      st = 20;
      break;
    case DND:
      st = stateDND;
      break;
    case HK:
      st = stateHK;
      break;
    default:
      break;
  }
}

void setup() {
  
  lcd.begin(EZM_BAUD_RATE);
  lcd.string(1, " ");
  
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);
  
  state = Initial;
  strcpy(roombuf, room);
}

void loop() {
  lcd.wstack(0);
  
  //Runs the state machine
  Tick_State(); 
  writeStatus(st);
}

//Write status to QT

/*
void writeStatus(int x){
  
  static char buffer[UDP_TX_PACKET_MAX_SIZE];
  for(int i = 0; i < UDP_TX_PACKET_MAX_SIZE; ++i) buffer[i] = 0; 
  
  itoa(x, buffer, 10);
  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.write(buffer);
  Udp.endPacket();
}

//Read from QT

void readStatus(){
  int packetSize = Udp.parsePacket();
  if(packetSize){
    
    static char buffer[UDP_TX_PACKET_MAX_SIZE];
    for(int i = 0; i < UDP_TX_PACKET_MAX_SIZE; ++i) buffer[i] = 0; 
    Udp.read(buffer,UDP_TX_PACKET_MAX_SIZE);
    
    r = atoi(buffer);
  }
}
*/

void writeStatus(int x){
  int packetSize = Udp.parsePacket();
  if(packetSize){
    
    static char buffer[UDP_TX_PACKET_MAX_SIZE];
    //static char buffer2[UDP_TX_PACKET_MAX_SIZE];
    
    for(int i = 0; i < UDP_TX_PACKET_MAX_SIZE; ++i) buffer[i] = 0; 
    Udp.read(buffer,UDP_TX_PACKET_MAX_SIZE);
    
    r = atoi(buffer);
    if(r != 100  && r != x){
      x = r;
      //st = r;
      state = Select;
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

void HomeM(){ // result 4 = Home screen
  delay(200);
  lcd.cls(BLACK);
  lcd.picture(0,0, "BGround2.gif");
  lcd.color(BLACK); 
  lcd.xy("cc");
  lcd.font("Serif72");
  lcd.println(strcat(room,"\"\"cc"));
  lcd.touchZone(1,20, 20, 280,200, 1);
}

void HomeD(){ // result 4 = Home screen
  delay(200);
  lcd.cls(BLACK);
  lcd.picture(0,0, "BGround5.jpg");
  lcd.color(BLACK); 
  lcd.xy("cc");
  lcd.font("Serif72");
  lcd.println(strcat(room,"\"\"cc"));
  lcd.touchZone(1,20, 20, 280,200, 1); 
}

void HomeH(){ // result 4 = Home screen
  delay(200);
  lcd.cls(BLACK);
  lcd.picture(0,0, "BGround6.jpg");
  lcd.color(BLACK); 
  lcd.xy("cc");
  lcd.font("Serif72");
  lcd.println(strcat(room,"\"\"cc"));
  lcd.touchZone(1,20, 20, 280,200, 1);
}

void Choice(){ //result 1 = pick a sign
  lcd.cls(BLACK);
  lcd.picture("Check.gif");
  lcd.button(2,217,35,41,41,4,4,5,1,1);
  lcd.button(3,217,102,41,41,4,4,5,1,1);
  lcd.button(4,217,170,41,41,4,4,5,1,1);
  lcd.xy(53, 35);
  lcd.color(BLACK);
  lcd.font("Sans24");
  lcd.println("Home");
  lcd.xy(45, 50);
  lcd.color(BLACK);
  lcd.font("Sans24");
  lcd.println("Screen");
  lcd.xy(50, 100);
  lcd.color(BLACK);
  lcd.font("Sans24");
  lcd.println("Do Not");
  lcd.xy(45, 120);
  lcd.color(BLACK);
  lcd.font("Sans24");
  lcd.println("Disturb");
  lcd.xy(50, 165);
  lcd.color(BLACK);
  lcd.font("Sans24");
  lcd.println("House");
  lcd.xy(45, 185);
  lcd.color(BLACK);
  lcd.font("Sans24");
  lcd.println("Keeping");
}
