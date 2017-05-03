#include <Adafruit_IO_Arduino.h>

#include <Adafruit_IO_Client.h>
#include <Adafruit_IO_FONA.h>

// pin used to control the servo
#define FEEDER_PIN 2

// create an instance of the servo class
Servo feeder;

// FEED DECLARATION
//AdafruitIO_Feed *PetFeeder = io.feed("PetFeeder");
AdafruitIO_Feed *Switcher = io.feed("Switcher");
//AdafruitIO_Feed *LED0_1 = io.feed("LED0_1");
//AdafruitIO_Feed *LED2_3 = io.feed("LED2_3");
//AdafruitIO_Feed *TVP_M = io.feed("TVPOWER_MUTE");
//AdafruitIO_Feed *TVUP = io.feed("TVUPVolume_Channel");
//AdafruitIO_Feed *TVDOWN = io.feed("TVDOWNVolume_Channel");
//FEED DECLARATION
//************STRUCT "Feed" definition below*****************//
struct feed{
  AdafruitIO_Feed *feed;
  feed->onMessage(eventHandler);
  //char[2] sig; //the sig is sent first to identify the component
  char sig;//the second char is controlled by switcher, and the final is state
  //int data;
}
PetFeeder = {.feed = io.feed("PetFeeder"), .sig = 's'},//{'s','p'} },
//Switcher = {.feed = io.feed("Switcher"), .sig = 'w'}//.sig={'',''} },
LED0_1 = {.feed = io.feed("LED0_1"), .sig = l},//.sig={'',''} },
TVP_M = {.feed = io.feed("TVPOWER_MUTE"), .sig = 'p'}  //.sig={'',''} },
TVUP = {.feed = io.feed("TVUPVolume_Channel"), .sig = 'u'}//.sig={'',''} },
TVDOWN = {.feed = io.feed("TVDOWNVolume_Channel"), .sig = 'd'}//.sig={'',''} },
;//Immediate feed declaration

//Struct "Feed"//
void setup() {

  // start the serial connection
  Serial.begin(9600); //what the fuck is 115200???

  // wait for serial monitor to open
  while(! Serial);

  // tell the servo class which pin we are using
  feeder.attach(FEEDER_PIN);

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // set up a message handler for the 'servo' feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  Switcher->onMessage(switcherHandler);
/*  PetFeeder->onMessage(PetFeederHandler);
  Switcher->onMessage(SwitcherHandler);
  LED0_1->onMessage(LED0_1Handler);
  TVP_M->onMessage(TVP_MHandler);
  TVUP->onMessage(TVUPHandler);
  TVDOWN->onMessage(TVDOWNHandler);*/

  //CONSIDER MAKING STRUCT "FEED" WHICH CONTAINS POINTER TO OBJECT
  //AS WELL AS THE HANDLER FUNCTION AND THE APPROPRIATE COMMANDS.
  //THIS COULD MAKE IT A LOT EASIER.

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

}

void loop() {
  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();
}

// this function is called whenever a 'servo' message
// is received from Adafruit IO. it was attached to
// the servo feed in the setup() function above.
/*void PetFeederHandler(AdafruitIO_Data *data) {
// convert the data to integer
int angle = data->toInt();
// make sure we don't exceed the limit
// of the servo. the range is from 0
// to 180.
if(angle < 0)
angle = 0;
else if(angle > 180)
angle = 180;
servo.write(angle);
}*/
void switcherHandler(AdafrutIO_Data *data)
{

}

void handler(AdafrutIO_Data *datum)
{

}
