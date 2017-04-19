#include <Time.h>

// ExecutorSlave module code for ApertureAutomation
//Copyright under Aperture Science on April 13, 2017
#include <Servo.h>

#include <boarddefs.h>
//#include <ir_Lego_PF_BitStreamEncoder.h>
#include <IRremote.h>
#include <IRremoteInt.h>

#include <Wire.h>


//***********************PARCEL*************************
//struct bitField{uint32_t val : 24;} command ={'0'+'0'+'0'};
struct parcel{
  char command[3] = {'0','0','0'}; // chars: {component , adressNumber , action}
};
boolean switcher = 0;
/* COMMAND KEY
Component: 'l' for light, 's' for servo, 'r' for ir, 
AdressNumber: different for each component OF THE SAME TYPE. So, every LED has a different adress
action: 'h' (high) for ledOn, 'l'(low) for ledOff, 'p' for tv power, 'c' for channel up, 'b' for channel down, 'v'for volume up, 'u' for volume down,
'm' for tv mute, a number for servo angle (!)
(8 bits is more than enough for 0-180, and one bit for +/-, so from -180 to 180
KEY */

/*************************** Hardware shit ************************************/
int lights[3] = {2,5,6};//pins. Position in array is the light's number, starting with light 0
//consider slider for lights so that they can be dimmed
Servo lock , petFeeder;
Servo servos[2] = {lock,petFeeder};

IRsend irSend; // irSend.sendSony(code,numberOfBits). The hex codes are in a txt file in this folder, and the #ofBits is the number of hex digits times 4
//remember that sony makes you send the code thrice

#define PIEZO 12
#define SNOOZE 13
int alarmTimes[3][2]=//first array is array of alarms, sub array is {hour,minute} in 24 hour format
{//3 alarms: 7:00 am, 7:30 am, 13:01 aka 1:01 pm
  {7,00} , {7,30} , {13,01}
  };
//time_t last;
/*******************************************************************************/
void setup()
{
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);
  //pinModes
  pinMode(PIEZO, OUTPUT);
  pinMode(SNOOZE, INPUT); //don't forget a 10k resistor
  for(int i=0; i < sizeof(lights)/sizeof(int);i++) {pinMode(lights[i], OUTPUT); Serial.println("LED INITIALIZED");}
  for(int i=0; i < sizeof(servos)/sizeof(Servo);i++) {servos[i].attach(i+7); Serial.println("SERVO INITIALIZED");} //SERVOS WILL BE ON PINS 7,8,9, etc.
}                                                                                                        //note to self: check that servos are on pwm pins

void loop()
{
  if(int s= second() < 3)//I'm too lazy to do a check for how recently the alarm went off
    {
    //for a< number of alarms
    for(int a=0;a<3;a++)
    {
      int h = hour();
      int m = minute();
      if (alarmTimes[a][0] == h && alarmTimes[a][1] == m) alarm();//if the hour and minute match, alarm
        }//for
    }//if seconds < 3
  delay(100);
}//loop

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  Serial.print("recieved: ");
  //while (1 < Wire.available()) // loop through all but the last
  struct parcel temp;
for(int i=0;i<3;i++)
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
    temp.command[i] = c;
  }
  Serial.println();
 // int x = Wire.read();    // receive byte as an integer
 // Serial.println(x);         // print the integer
 execute(temp);
}//recieveEvent

void execute(struct parcel p)
{
 Serial.print("Executing parcel: "); Serial.print(p.command[0] + p.command[1]+p.command[2]);
  switch(p.command[0])//switch for component type
   {
     case 'w' ://switcher
                switcher = p.command[2] == '0'? false:true;
                break;//no use analyzing further, move to next parcel
     case 'l' ://light
                digitalWrite(lights[p.command[1]], (p.command[2] == 'h')? HIGH:LOW); 
                break;//light. writes to lights[adress#] high if command[2] is 'h'.
     case 'r' ://IR
                switch(p.command[2])
                      {
                        case 'p': for(int i=0 ; i<3; i++){irSend.sendSony(0xA90,12); delay(40);}; break; //power
                        case 'c': for(int i=0 ; i<3; i++) {irSend.sendSony(0x90,8); delay(40);}; break; //channel up
                        case 'b': for(int i=0 ; i<3; i++) {irSend.sendSony(0x890,12); delay(40);}; break; //channel down
                        case 'v': for(int i=0 ; i<3; i++) {irSend.sendSony(0x490,12); delay(40);}; break; //volume up
                        case 'u': for(int i=0 ; i<3; i++) {irSend.sendSony(0xC90,12); delay(40);};break; //volume down
                        case 'm': for(int i=0 ; i<3; i++)  {irSend.sendSony(0x290,12); delay(40);}; break;//mute
                        default: Serial.println("unidentifyable tv command");
                       }//TV subswitch ending
     case 's' ://servo
              Serial.print(F("Setting servo state: ")); Serial.println(p.command[2] == 'f'?"forward":"neutral");
              //servos[p.command[1]].write(p.command[2]);
              if(p.command[1]=='p') adjustServo(petFeeder,p.command[2]);
              if(p.command[1] == 'l') adjustServo(lock,p.command[2]);
              
              break;

     default: Serial.print("UNACCEPTABLE PARCEL: "); for(int i=0;i<3;i++) {Serial.println(p.command[i]);} Serial.println();
                 
     }//switch
                    
  }//execute

void adjustServo(Servo s, char c)
{
  if(c=='f'){s.write(90);} else {s.write(90);}
  delay(15);
  }

void alarm() //set off piezo, stop when button pressed
{
  digitalWrite(PIEZO,HIGH);
  while(digitalRead(SNOOZE) == 0) {;}//will keep checking until buttonis pressed
  //last = now();//mark when the last alarm went off
  }
