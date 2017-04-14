/***************************************************
  Adafruit MQTT Library Ethernet Example

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Alec Moore
  Derived from the code written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <Wire.h>

#include <SPI.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include <Ethernet.h>
#include <EthernetClient.h>
#include <Dns.h>
#include <Dhcp.h>

/************************* Ethernet Client Setup *****************************/
byte mac[] = {0xCC, 0xAE, 0xBD, 0xFF, 0xFD, 0xDA}; //just some number that no other computer has

//Uncomment the following, and set to a valid ip if you don't have dhcp available.
IPAddress iotIP (192, 168, 0, 42);
//Uncomment the following, and set to your preference if you don't have automatic dns.
//IPAddress dnsIP (8, 8, 8, 8);
//If you uncommented either of the above lines, make sure to change "Ethernet.begin(mac)" to "Ethernet.begin(mac, iotIP)" or "Ethernet.begin(mac, iotIP, dnsIP)"


/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "rhausman"
#define AIO_KEY         "a4ef95bb0bdc46daaeb7a608d60d4426"


/************ Global State (you don't need to change this!) ******************/

//Set up the ethernet client
EthernetClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// You don't need to change anything below this line!
#define halt(s) { Serial.println(F( s )); while(1);  }


/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
//Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/photocell");

// Subscribing to changes for command feeds
Adafruit_MQTT_Subscribe LEDToggle = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/LEDToggle");
Adafruit_MQTT_Subscribe LED2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/LED2");
Adafruit_MQTT_Subscribe TVPower = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/TVPower");
Adafruit_MQTT_Subscribe TVChannelUP = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/TVChannelUP");
Adafruit_MQTT_Subscribe TVChannelDOWN = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/TVChannelDOWN");
Adafruit_MQTT_Subscribe TVVolumeUP = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/TVVolumeUP");
Adafruit_MQTT_Subscribe TVVolumeDOWN = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/TVVolumeDOWN");
Adafruit_MQTT_Subscribe TVMUTE = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/TVMUTE");

//NOTE TO SELF: TRY TO REPLACE THE VOLUME AND CHANNEL UPDOWN WITH A SLIDER USING PROTOCOLS IN FOLDER
//IN and PUBLISHING feed arrays
#define numSubscriptions 8
Adafruit_MQTT_Subscribe inArray[numSubscriptions] = {LEDToggle, LED2 , TVPower, TVChannelUP , TVChannelDOWN , TVVolumeUP , TVVolumeDOWN , TVMUTE };
/*************************** Hardware shit ************************************/
/*
 * 
     struct light{
      Adafruit_MQTT_Subscribe feed;
      int pin;
    } light1 = {.feed = LEDToggle, .pin=2}, light2={.feed=LED2,.pin=5};
    struct light lights[2] = { light1,light2 };//pairings of led feeds and the pin that the light is on
*/
/*************************** Sketch Code ************************************/

void setup() {
  //Serial.begin(115200); WTF???
  Wire.begin();
  Serial.begin(9600);
  Serial.println(F("Adafruit MQTT demo"));

  // Initialise the Client
  Serial.print(F("\nInit the Client..."));
  if(Ethernet.begin(mac) == 0) Ethernet.begin(mac, iotIP);
  
  delay(1000); //give the ethernet a second to initialize
  
//SET UP SUBSCRIPTIONS TO READ FEEDS
  for(int i=0; i<numSubscriptions; i++) mqtt.subscribe(&inArray[i]);//the i terminator should match num of in feeds

//setting input and output pins
//for(int i=0; i< (sizeof(lights)/ sizeof(struct light)); i++){pinMode(lights[i].pin , OUTPUT);}
}//setup

uint32_t x=0;

void loop() {//do we need "io.run()"? not sure, see servo skectch, but if it ain't broke don't fix it
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();
//CONSIDER USING EVENT HANDLERS AS OUTLINED IN "adafrutio_16_servo.ino" example instead of the hodgepoge below
 for(int i=0;i<numSubscriptions; i++) {refreshData(); Serial.print("Feed "+i); Serial.println((char*)inArray[i].lastread);}//look for packets on all subscription feeds
  sendData();//this is where everything happens

  // ping the server to keep the mqtt connection alive
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print(F("Connecting to MQTT... "));

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println(F("Retrying MQTT connection in 5 seconds..."));
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
  }
  Serial.println(F("MQTT Connected!"));
}

//GET DATA FROM ALL FEED AND PUT THEM INTO THE 'lastread' vals of the suscription objects
 // this is our 'wait for incoming subscription packets' busy subloop
void refreshData(){ 
  //Adafruit_MQTT_Subscribe *subscription;
   // subscription = 
   int i=0;
   while( (mqtt.readSubscription(1000) != '\0') && i<100) {i++;}
   //if it never returns null for some reason it will stop after 100 reps
   //each time it excecutes, it checks if there is none left
 /*   while() {
    if (subscription == &onoffbutton) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);
    }
  }//while */
}//refreshData

void sendData() {
  //LEDToggle, LED2 , TVPower, TVChannelUP , TVChannelDOWN , TVVolumeUP , TVVolumeDOWN , TVMUTE
   Wire.beginTransmission(8);
   sendByByte(*LEDToggle.lastread == 1 ? "l0h":"l0l");//LEDToggle (aka LED 0)
   //INSERT AN LED1
   sendByByte(*LED2.lastread == 1 ? "l2h":"l2l" );
   sendByByte(*TVPower.lastread == 1 ? "t0p":"");
   sendByByte(*TVChannelUP.lastread == 1 ? "t0c":"");
   Wire.endTransmission(); 
  }//

void sendByByte(String s){if(s[0]!='\0') for(int i=0;i<s.length();i++){Wire.write((char) s[i]);};}
