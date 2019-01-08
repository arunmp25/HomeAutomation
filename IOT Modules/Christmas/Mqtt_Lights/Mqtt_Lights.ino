/***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <ArduinoJson.h>

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "###"
#define WLAN_PASS       "#####"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "192.168.1.14"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    ""
#define AIO_KEY         ""

/************ RELAY SET UP ***************************************************/

#define relay1 D1;
#define relay2 D2;
#define relay3 D3;
#define relay4 D4;

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish pubChannel = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/pubChannel");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe subChannel = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/subChannel");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&subChannel);

  //Now initialize Relay pins

  pinMode(D1, OUTPUT); // initialize pin as OUTPUT
  pinMode(D2, OUTPUT); 
  pinMode(D3, OUTPUT); 
  pinMode(D4, OUTPUT);
  digitalWrite(D1, LOW); 
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW); 
  digitalWrite(D4, LOW);
}

uint32_t x=0;
StaticJsonBuffer<500> jsonBuffer;
int pattern = 4;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &subChannel) {
      JsonObject& jsonBody = jsonBuffer.parseObject((char *)subChannel.lastread);
      if(""!=jsonBody["pattern"]){
         pattern = jsonBody["pattern"];
         Serial.println(pattern);
      }
    }
  }

  if(pattern==0){
      allOff();
  } else if(pattern==1){
      patternOne();
  } else if(pattern==2){
      patternTwo();
  } else if(pattern==3) {
      patternThree();
  } else if(pattern==4){
    patternOne();
    allOn();

    delay(2000);
    patternTwo();
    delay(2000);
    patternTwo();
    delay(2000);
    allOn();
    
    delay(2000);
    patternThree();
    delay(2000);
    patternThree();
    delay(2000);
    allOn();
  } else {
      defaultPattern();    
  }

  // Now we can publish stuff!
  Serial.print(F("\nSending photocell val "));
  Serial.print(x);
  Serial.print("...");
  if (! pubChannel.publish(x++)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
}

void allOn(){
   digitalWrite(D1, LOW); 
   digitalWrite(D2, LOW);
   digitalWrite(D3, LOW); 
   digitalWrite(D4, LOW);
}

void allOff(){
   digitalWrite(D1, HIGH); 
   digitalWrite(D2, HIGH);
   digitalWrite(D3, HIGH); 
   digitalWrite(D4, HIGH);
}

void patternOne(){
   Serial.print(F("Inside Pattern 1"));
   allOn();
   digitalWrite(D1, LOW);
   digitalWrite(D2, HIGH);
   digitalWrite(D3, HIGH);
   digitalWrite(D4, HIGH);
   delay(2000);
   digitalWrite(D1, HIGH);
   digitalWrite(D2, LOW); 
   digitalWrite(D3, HIGH);
   digitalWrite(D4, HIGH);
   delay(2000);
   digitalWrite(D1, HIGH);
   digitalWrite(D2, HIGH);
   digitalWrite(D3, LOW);
   digitalWrite(D4, HIGH);
   delay(2000);
   digitalWrite(D1, HIGH);
   digitalWrite(D2, HIGH);
   digitalWrite(D3, HIGH);
   digitalWrite(D4, LOW);
   delay(2000);
   allOn();
}

void patternTwo(){
   Serial.println("Inside Pattern 2");
   digitalWrite(D1, HIGH);
   digitalWrite(D2, LOW);
   digitalWrite(D3, HIGH);         
   digitalWrite(D4, LOW);
   delay(2000);        
   digitalWrite(D1, LOW);
   digitalWrite(D2, HIGH);
   digitalWrite(D3, LOW);         
   digitalWrite(D4, HIGH);
}

void patternThree(){
   Serial.println("Inside Pattern 3");
   digitalWrite(D1, LOW);
   digitalWrite(D2, LOW);
   digitalWrite(D3, HIGH);         
   digitalWrite(D4, HIGH);
   delay(2000); 
   digitalWrite(D1, HIGH);
   digitalWrite(D2, HIGH);
   digitalWrite(D3, LOW);         
   digitalWrite(D4, LOW);
}

void defaultPattern(){
   Serial.println("Inside default pattern");
   digitalWrite(D1, HIGH); 
   digitalWrite(D2, HIGH);
   digitalWrite(D3, HIGH); 
   digitalWrite(D4, HIGH);
   delay(2000);
   digitalWrite(D1, LOW); 
   digitalWrite(D2, LOW);
   digitalWrite(D3, LOW); 
   digitalWrite(D4, LOW);
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
