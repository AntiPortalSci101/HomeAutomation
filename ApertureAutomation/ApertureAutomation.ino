// pin used to control the servo
#define FEEDER_PIN 2

// create an instance of the servo class
Servo feeder;

// FEED DECLARATION
AdafruitIO_Feed *PetFeeder = io.feed("PetFeeder");
AdafruitIO_Feed *Switcher = io.feed("Switcher");
AdafruitIO_Feed *LED0_1 = io.feed("LED0_1");
//AdafruitIO_Feed *LED2_3 = io.feed("LED2_3");
AdafruitIO_Feed *TVP_M = io.feed("TVPOWER_MUTE");
AdafruitIO_Feed *TVUP = io.feed("TVUPVolume_Channel");
AdafruitIO_Feed *TVDOWN = io.feed("TVDOWNVolume_Channel");
//FEED DECLARATION
//************HARDWARE PIN DECLARATION below*****************//
int lights[] = 
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
  servo_feed->onMessage(handleMessage);

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
void handleMessage(AdafruitIO_Data *data) {

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

}
