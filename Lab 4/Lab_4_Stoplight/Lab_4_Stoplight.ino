/*
 * Created by Aaron Nelson
 * Lab 4 Event Bus
 * 10-26-19
 * This script is used with the stoplight module. It will light up different colors based on what is passed to the MQTT server (posted from an ultrasonic sensor).
 *  
 * The MQTT portion of this script was obtained from https://arduinodiy.wordpress.com/2017/11/24/mqtt-for-beginners/
 *
    Stoplight Pins:
        Green: Pin12 (D6)
        Yellow: Pin13 (D7)
        Red: Pin15 (D8)
        GND: GND
*/

#include <ESP8266WiFi.h>            // This contains the libraries that allows the board to connect to wifi
#include <PubSubClient.h>           // This is used for communication with the MQTT Server

const char* ssid = "YOUR WIFI HERE";       // Specify the name of your wifi
const char* password = "YOUR PASSWORD HERE";      // Specify the password for your wifi

const char* mqttServer = "192.168.137.1"; // The location of your MQTT Server
const int mqttPort = 1883;                // The port number your MQTT Server is running on (1883 is the default for Mosquitto) 

unsigned long beginningMillis;      // A variable to hold our current time (used for auto mode to keep track of how much time has passed)
int onBlink;                        // Set if the server reading sent 'b'. Will change the red light on and off

WiFiClient stoplightClient;         // Name for our MQTT connection client
PubSubClient client(stoplightClient);     // Creates a partially initialized client instance

int green = 12;                     // Pin D6 is used for green. If using a different pin, specify that here
int yellow = 13;                    // Pin D7 is used for yellow. If using a different pin, specify that here
int red = 15;                       // Pin D8 is used for red. If using a different pin, specify that here
int redFlash = 0;                   // While this value is 1, it specifies the light should be red, while 0, it is off during the flashing stage


void setup() {                      // This runs when the board is first turned on
  Serial.begin(115200);             // This allows serial information when connected to a computer (this will tell what echosensor message was sent to the MQTT server)
  pinMode(green, OUTPUT);           // Prepares the pin connected to the green light for output
  pinMode(yellow, OUTPUT);          // Prepares the pin connected to the yellow light for output
  pinMode(red, OUTPUT);             // Prepares the pin connected to the red light for output
  lightsOff();                      // Initialize the device by turning all of the stoplight lights off

  /* Connect to WiFi */
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");   // This is shown on the serial if connected to a computer
  Serial.println(ssid);             // It displays the wifi it is trying to connect to

  WiFi.mode(WIFI_STA);              // It sets the wifis mode to "Station" (rather than "AP" which is the Access Point mode)
  WiFi.hostname("Stoplight");       // Hostname to uniquely identify our device
  WiFi.begin(ssid, password);       // Attempts to connect to wifi using provided credentials

  while (WiFi.status() != WL_CONNECTED) {     // While the wifi is not connected yet:
    delay(500);                     // every half second,
    Serial.print(".");              // it prints a dot on the Serial Monitor to show it is still trying to connect
  }
  Serial.println("");
  Serial.println("WiFi connected"); // When it does connect, show that there was success

  connectMQTT();                    // Runs the function to set up our MQTT connection
  
  // Blink LED quickly to show that it is connected to wifi and the MQTT server
  digitalWrite(LED_BUILTIN, LOW);   // light turns on
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);  // light turns off
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);   // light turns on
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);  // light turns off
  
  client.publish("device/stoplight", "Activated");  // Post in the device channel that this device is on
}





void loop() {                       // This constantly cycles through, checking the MQTT server for a message, reconnecting if it becomes disconnected, and blinking the red light if that setting is on
  client.loop();                    // Runs through the MQTT functions to process incoming messages and send publish data

  if (!client.connected()){         // Occasionally the client drops connection randomly and stops working
    Serial.println("MQTT was Unexpectedly Disconnected");
    connectMQTT();                  // If the connection dropped, try to reconnect to our server
  }  

  if (onBlink == 1){                // If the red light is supposed to be blinking
    if ((325 + beginningMillis) < millis()) {       // 325ms after the light last blinked
      if (redFlash == 0){           // If the red light was off,
          redLight();               // turn the red light on
          redFlash = 1;             // and change the variable to have it turn off next time
      } else if (redFlash == 1) {   // If the red light was on,
          lightsOff();              // turn the red light off
          redFlash = 0;             // and change the variable to ahve it turn on next time
      }
      beginningMillis = millis();   // Restart the counter for when the loop will run again
      delay(5);
    }
  }
}





void connectMQTT() {              // When first connecting to MQTT or when it becomes disconnected, run this script
  client.setServer(mqttServer, mqttPort);     // Setup the server connection with information the user provided
  client.setCallback(callback);   // Setup the callback to listen to changes made on the MQTT server
  while(!client.connected()) {    // If the MQTT server is not connected or becomes disconnected, try again
    Serial.println("connecting to MQTT...");
    if (client.connect("Stoplight")) {        // Try to connect with this name
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("failed... Re");           // Repeatedly says "failed... Reconnecting" as this loop cycles through
      // Serial.print(client.state());           // Show in the Serial Monitor what the current state of the server connection is
      delay(2000);                // Wait two seconds before trying to connect to the server again
    }
  } 
  client.subscribe("garage/echoSwitch/color");  // Listen to this path on the server and when a change is noticed, run the callback function
}





void callback(char* topic, byte* payload, unsigned int length)
{
  // The callback listens to the MQTT server and checks for changes in the "garage/echoSwitch/color"
  // This function occurs after a message was received
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);          // Notify which subscribed path received a change
  Serial.print("Message:");

  String value = "";              // To contain the value from the MQTT path
  for(int i = 0; i < length; i++) {
    char ch = ((char)payload[i]); // Only one character is read at a time
    value.concat(ch);             // So create "value" based on the string of these characters
  }
  Serial.print(value);            // Show the value that came through in the serial monitor
  Serial.println();

  // Respond to the color the Echo Sensor sent to tell which color the light should turn
  if (value == "g") {
    // Echo sensor is over 35 cm
    greenLight();
    onBlink = 0;
  } else if (value == "y") {
    // Echo sensor is between 20-35 cm
    yellowLight();
    onBlink = 0;
  } else if (value == "r") {
    // Echo sensor is between 10-20 cm
    redLight();
    onBlink = 0;
  } else if (value == "b") {
    // Echo sensor is between 0-10 cm
    lightsOff();
    // Because this is too close, we wish to flash the red light (functionality for this is in loop())
    onBlink = 1;
    beginningMillis = millis();
  } else if (value == "o") {
    // The door switch became disconnected and the echo sensor is no longer reading values
    lightsOff();
    onBlink = 0;
  }
}





/* These are the functions that are called depending on the read MQTT value. 
    They only turn on the light they are supposed to according to the pin number specified at the beginning of the script */
void lightsOff() {
  digitalWrite(green, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(red, LOW);
}

void greenLight() {
  digitalWrite(green, HIGH);
  digitalWrite(yellow, LOW);
  digitalWrite(red, LOW);
}

void yellowLight() {
  digitalWrite(green, LOW);
  digitalWrite(yellow, HIGH);
  digitalWrite(red, LOW);
}

void redLight() {
  digitalWrite(green, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(red, HIGH);
}
