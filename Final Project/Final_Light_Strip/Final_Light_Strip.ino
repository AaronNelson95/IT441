/*
 * Created by Aaron Nelson
 * Final Project - Arduino Alarm - Light Strip
 * 12-9-19
 * This script is to be used with a WS2812B LED light strip. It connects to the Adafruit.io MQTT server and listens for a value to come into the 'Alarm' feed. When a '1' comes in, it lights up all of the LED lights. A '0' turns off these lights.
 * 
 * 
 * The Adafruit.io portion of this script was obtained from the Arduino example (after adding the Adafruit Library) at File -> Examples -> Adafruit MQTT Library -> mqtt_esp8266_callback
 * 
 * The LED light strip portion was built from the example at https://www.norwegiancreations.com/2018/01/programming-digital-rgb-led-strips-with-arduino-and-the-fastled-library/
 *
    WS2812B LED light strip pins:
        Red Wire (power): +5VDC
        White Wire (ground): GND
        Green Wire (trigger): D6 (or any as specified in the code)
 */

#include <ESP8266WiFi.h>            // This contains the libraries that allows the board to connect to wifi
#include "Adafruit_MQTT.h"          // Library to access data from an Adafruit.io online feed
#include "Adafruit_MQTT_Client.h"   // Used to subscribe to an Adafruit.io online feed
#include "FastLED.h"                // Library to manipulate light strip colors


/************************* WiFi Access Point *********************************/
const char* ssid     = "ENTER WIFI HERE";       // Specify the name of your wifi
const char* password = "ENTER PASSWORD HERE";   // Specify the password for your wifi


/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER      "io.adafruit.com"       // Pulling data from the Adafruit website
#define AIO_SERVERPORT  1883                    // use 8883 for SSL
#define AIO_USERNAME    "ENTER ADAFRUIT NAME"   // Username for Adafruit (goes before the /feed/# MQTT feed)
#define AIO_KEY         "ENTER ADAFRUIT KEY"  	// Obtained by going to io.adafruit.com and clicking AIO Key link in top right. Copy the "Active Key" here


/****************************** Feeds ***************************************/
// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Watch a feed called 'alarm' and subscribe to changes.
Adafruit_MQTT_Subscribe alarm = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Alarm");


 /*********************** Light Strip Settings ******************************/
// How many leds in your strip?
#define NUM_LEDS 15    // this actually has 150, up to 30 should be safe to use through the board's voltage
#define DATA_PIN D6    // 2 is pin D4, but this will turn on the onboard LED as well (which isn't desired)f
CRGB leds[NUM_LEDS];  // Sets up the lights on the strip (up through the number specified)

int lightShow = 0;    // Variable that manages if the special "Light" command was passed in the feed (just a fun demo mode)





void setup() { 
  Serial.begin(115200);             // This allows serial information when connected to a computer
  
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);

  /* Connect to WiFi */
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");   // This is shown on the serial if connected to a computer
  Serial.println(ssid);             // It displays the wifi it is trying to connect to

  WiFi.mode(WIFI_STA);              // It sets the wifis mode to "Station" (rather than "AP" which is the Access Point mode)
  WiFi.hostname("Light Strip");     // Hostname to uniquely identify our device
  WiFi.begin(ssid, password);       // Attempts to connect to wifi using the provided credentials

  while (WiFi.status() != WL_CONNECTED) {   // While the wifi is not connected yet:
    delay(500);                     // every half second,
    Serial.print(".");              // it prints a dot on the Serial Monitor to show it is still trying to connect
  }
  Serial.println("");
  Serial.println("WiFi connected"); // When it does connect, show that there was success

  mqtt.subscribe(&alarm);           // Setup MQTT subscription for alarm feed

  /* Bounce the LED Light colors to show it is connected to the internet */
  for (int i=0; i < NUM_LEDS; i++) {
    // One by one, turn on each LED light and keep it on
    leds[i] = CRGB::White; 
    FastLED.setBrightness( 50 );
    FastLED.show();
    delay(50);
  }
  delay(1000);
  for (int i=NUM_LEDS; i > -1; i--) {
    // One by one, turn off each LED light in the reverse order they turned on
    leds[i] = CRGB::Black; 
    FastLED.show();
    delay(50);
  }
}




 
void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &alarm) {         
      // If something came through the "alarm" feed in Adafruit.io
      Serial.print(F("Got: "));
      String information = "";        // A string to insert the characters read by the feed
      char* ch = (char *)alarm.lastread;
      information.concat(ch);         // Add the last read character to the information string to get the full value
      Serial.println(information);    // Show the value that came in to the Serial Monitor

      // A "1" was received (the alarm is on)
      if (information == "1") {
        // This simply turns on every light in the LED strip to white (at 50 brightness)
        Serial.println("Starting the alarm!");
        lightShow = 0;
        for (int i=0; i < NUM_LEDS; i++) {
          leds[i] = CRGB::White; 
          FastLED.setBrightness( 50 );
          FastLED.show();
          // delay(50);   // enable to turn the lights on one by one
        }
      }

      // A "0" was received (the alarm was turned off)
      if (information == "0") {
        // This simply turns off every light in the strip
        Serial.println("Ending the alarm!");
        lightShow = 0;
        for (int i=NUM_LEDS; i > -1; i--) {
          leds[i] = CRGB::Black; 
          FastLED.show();
          // delay(50);   // enable to turn the lights off one by one
        }
      }

      // The word "Lights" was received
      if (information == "Lights") {
        // This turns on a light show that will bounce the path the LED lights turn on and off, end to end
        Serial.println("Showing Lights Show");
        lightShow = 1;
      }
    }
  }

  // This is a special "demo" that will flash the lights- having them turn on one by one until they are all on, and then they will turn off in reverse
  // It is only activated when the word "Lights" is passed into the alarm feed
  if (lightShow == 1) {
    for (int i=0; i < NUM_LEDS; i++) {
      // One by one, turn on each LED light and keep it on
      leds[i] = CRGB::White; 
      FastLED.setBrightness( 50 );
      FastLED.show();
      delay(50);
    }
    delay(2000);
    for (int i=NUM_LEDS; i > -1; i--) {
      // One by one, turn off each LED light in the reverse order they turned on
      leds[i] = CRGB::Black; 
      FastLED.show();
      delay(50);
    }
  }
}





// Function to connect and reconnect as necessary to the Adafruit.io MQTT server
// Should be called in the loop function and it will take care of connecting and reconnecting
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected and return back to the loop
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to Adafruit... ");

  uint8_t retries = 3;                  // It will attempt to connect 3 times before predicting the internet is down
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying Adafruit connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;    // Count against one of the retires becuase there was no success
       if (retries == 0) {
         // basically die and wait for the board to be reset
         while (1);
       }
  }
  Serial.println("Adafruit Connected!");
}
