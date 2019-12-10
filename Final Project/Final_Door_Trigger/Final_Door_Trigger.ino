/*
 * Created by Aaron Nelson
 * Final Project - Arduino Alarm - Door Trigger
 * 12-9-19
 * This script is to be used with a reed switch. It connects to the Adafruit.io MQTT server and listens for a value to come into the 'Alarm' feed. When a '1' comes in and the door opens, it should publish a value of '0' to turn off the alarm.
 * 
 * 
 * The Adafruit.io portion of this script was obtained from the Arduino example (after adding the Adafruit Library) at File -> Examples -> Adafruit MQTT Library -> mqtt_esp8266_callback
 * 
    ReedSwitch Pins:
        One Wire: Pin12 (D6)
        The Other Wire: GND
 */

#include <ESP8266WiFi.h>            // This contains the libraries that allows the board to connect to wifi
#include "Adafruit_MQTT.h"          // Library to access data from an Adafruit.io online feed
#include "Adafruit_MQTT_Client.h"   // Used to subscribe to an Adafruit.io online feed


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

// Prepare the feed 'alarm' to publish a '0' when the alarm should be turned off.
Adafruit_MQTT_Publish alarmOff = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Alarm");


 /*********************** Reed Switch Settings ******************************/
int reed_switch = 12;               // Pin D6 is connected to one of the switch wires. If using a different pin, specify that here
int reed_status = 0;                // When this is 0, it means it is connected to the magnet. A 1 means it is disconnected and seperated
int initial_reed_status = 0;        // Keeps the last value of the reed magnet (to only publish to the sensor when a CHANGE is made)
int listeningToSwitch = 0;          // When this is a 1 (alarm is on), it listens to the door switch and turns the alarm off if the door is opened




void setup() { 
  Serial.begin(115200);             // This allows serial information when connected to a computer
  
  pinMode(LED_BUILTIN, OUTPUT);     // Prepares the builtin light pin for output (which turns on when the reed magnet is connected)
  digitalWrite(LED_BUILTIN, HIGH);  // Initially, turn off the builtin LED light
  pinMode(reed_switch, INPUT_PULLUP);     // Initialize the reed switch pin for input. Pullup is necessary for when the magnet becomes reconnected

  /* Connect to WiFi */
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");   // This is shown on the serial if connected to a computer
  Serial.println(ssid);             // It displays the wifi it is trying to connect to

  WiFi.mode(WIFI_STA);              // It sets the wifis mode to "Station" (rather than "AP" which is the Access Point mode)
  WiFi.hostname("Door Switch");     // Hostname to uniquely identify our device
  WiFi.begin(ssid, password);       // Attempts to connect to wifi using the provided credentials

  while (WiFi.status() != WL_CONNECTED) {   // While the wifi is not connected yet:
    delay(500);                     // every half second,
    Serial.print(".");              // it prints a dot on the Serial Monitor to show it is still trying to connect
  }
  Serial.println("");
  Serial.println("WiFi connected"); // When it does connect, show that there was success

  mqtt.subscribe(&alarm);           // Setup MQTT subscription for alarm feed
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
        // Start listening to the door switch's status. If it is opened, turn off the alarm
        Serial.println("Starting the alarm!");
        digitalWrite(LED_BUILTIN, LOW);         // The LED light turns on
        reed_status = digitalRead(reed_switch); // Read the current value of the magnet switch
        initial_reed_status = reed_status;      // Store the initial value of the door (so if the door was opened when the alarm turned on, don't automatically turn it off)
        Serial.println(initial_reed_status);
        listeningToSwitch = 1;
      }

      // A "0" was received (the alarm was turned off)
      if (information == "0") {
        // Turn off the light and don't worry about collecting/publishing door data
        Serial.println("Ending the alarm!");
        digitalWrite(LED_BUILTIN, HIGH);       // The LED light turns off
        listeningToSwitch = 0;
      }
    }


  }

  if (listeningToSwitch == 1 && initial_reed_status == 0) {   // The alarm is on and the door is currently closed (sitch is together)
    reed_status = digitalRead(reed_switch);     // Check if the magnet is connected or not
    if (reed_status == 1) {                     // If the door goes from closed to opened
      Serial.println("switch just disconnected (the door opened)");
      if (! alarmOff.publish("0")) {            // Publish the value of "0" to turn the alarm off
        Serial.println(F("Failed to turn off alarm"));
      } else {
        Serial.println(F("Alarm should turn off soon"));
      }
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
       retries--;    // Count against one of the retries because there was no success
       if (retries == 0) {
         // basically die and wait for the board to be reset
         while (1);
       }
  }
  Serial.println("Adafruit Connected!");
}
