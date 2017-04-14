#include <Servo.h>

#include <boarddefs.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include <IRremote.h>
#include <IRremoteInt.h>

#include <Wire.h>
// ExecutorSlave module code for ApertureAutomation
//Copyright under Aperture Science on April 13, 2017



//***********************PARCEL*************************
//struct bitField{uint32_t val : 24;} command ={'0'+'0'+'0'};
struct parcel{
  char command[3] = {'0','0','0'}; // chars: {component , adressNumber , action}
};
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
Servo lock,feeder;
Servo servos[2] = {lock,feeder};

IRsend irSend; // irSend.sendSony(code,numberOfBits). The hex codes are in a txt file in this folder, and the #ofBits is the number of hex digits times 4
//remember that sony makes you send the code thrice


void setup()
{
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
  for(int i=0; i < sizeof(lights)/sizeof(int)) {pinMode(lights[i], OUTPUT); Serial.println("LED INITIALIZED");}
  for(int i=0; i < sizeof(servos)/sizeof(Servo)) {servos[i].attach(i+7); Serial.println("SERVO INITIALIZED");} //SERVOS WILL BE ON PINS 7,8,9, etc.
}

void loop()
{
  delay(100);
}

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
 
  switch(p.command[0])//switch for component type
   {
     case 'l' ://light
                digitalWrite(lights[p.command[1]], (p.command[2] == 'h')? HIGH:LOW); 
                break;//light. writes to lights[adress#] high if command[2] is 'h'.
     case 'r' ://IR
                switch(p.command[2])
                      {
                        case 'p': for(int i=0 ; i<3; i++){irSend.sendSony(0xA90,12); delay(40);}; break; //power
                        case 'c': for(int i=0 ; i<3; i++) {irSend.sendSony(0x90,8); delay(40);}; break //channel up
                        case 'b': for(int i=0 ; i<3; i++) {irSend.sendSony(0x890,12); delay(40);}; break; //channel down
                        case 'v': for(int i=0 ; i<3; i++) {irSend.sendSony(0x490,12); delay(40);}; break; //volume up
                        case 'u': for(int i=0 ; i<3; i++) {irSend.sendSony(0xC90,12); delay(40);};break; //volume down
                        case 'm': for(int i=0 ; i<3; i++)  {irSend.sendSony(0x290,12); delay(40);}; break;//mute
                        default: Serial.println("unidentifyable tv command");
                       }//TV subswitch ending
     case 's' ://servo
              Serial.println("Setting servo to angle" + p.command[2]);

     default: Serial.println("UNACCEPTABLE PARCEL");
                 
     }//switch
                
    
  }//execute
