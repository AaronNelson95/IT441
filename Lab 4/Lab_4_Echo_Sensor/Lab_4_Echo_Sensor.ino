/*
 * Created by Aaron Nelson
 * Lab 4 Event Bus
 * 10-26-19
 * This script is used for the ultrasonic sensor (HC-SR04). It first listens to the MQTT server (whether or not the reed switch is connected) and it posts sensor reading groupings to the server (to be red by the stoplight)
 * 
 * The MQTT portion of this script was obtained from https://arduinodiy.wordpress.com/2017/11/24/mqtt-for-beginners/
 * 
 * Original Script for the sensor functionality was created by Rui Santos, provided at:
 *      https://randomnerdtutorials.com/complete-guide-for-ultrasonic-sensor-hc-sr04/ 
 *
    Ultrasonic Sensor Pins:
        VCC: +5VDC
        Trig: Trigger (INPUT) - Pin13 (D7)
        Echo: Echo (OUTPUT) - Pin12 (D6)
        GND: GND
 */

#include <ESP8266WiFi.h>            // This contains the libraries that allows the board to connect to wifi
#include <PubSubClient.h>           // This is used for communication with the MQTT Server

const char* ssid = "YOUR WIFI HERE";       // Specify the name of your wifi
const char* password = "YOUR PASSWORD HERE";      // Specify the password for your wifi

const char* mqttServer = "192.168.137.1"; // The location of your MQTT Server
const int mqttPort = 1883;                // The port number your MQTT Server is running on (1883 is the default for Mosquito)  

WiFiClient echoClient;              // Name for our MQTT connection client
PubSubClient client(echoClient);    // Creates a partially initialized client instance

int trigPin = 13;                   // The trigger is on pin D7. If using a different pin, specify that here
int echoPin = 12;                   // The echo is on pin D6. If using a different pin, specify that here
long duration, cm, inches;          // This code can read distances in both cm and inches. They will both be printed to the Serial Monitor
int startSensor = 0;                // When this is a 1, it means the door magnet is connected and that the sensor should obtain distance readings

char color = 'o';                   // Contains the current color the stoplight should turn (to be posted to the MQTT server)
char lastColor = 'g';               // Contains the last color the stoplight should be (to only update on changed states)

void setup() {                      // This runs when the board is first turned on
  Serial.begin (115200);            // This allows serial information when connected to a computer (in here, this shows a constant stream of sensor readings)
  
  pinMode(trigPin, OUTPUT);         // Trigger acts as an output (it sends off a wave)
  pinMode(echoPin, INPUT);          // Echo acts as in input (it listens for the wave to come back)
  pinMode(LED_BUILTIN, OUTPUT);     // The LED light will work as an output and show when the sensor is currently collecting data
  digitalWrite(LED_BUILTIN, HIGH);  // Initially turn the built in LED light off
  
  /* Connect to WiFi */
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");   // This is shown on the serial if connected to a computer
  Serial.print(ssid);               // It displays the wifi it is trying to connect to

  WiFi.mode(WIFI_STA);              // It sets the wifis mode to "Station" (rather than "AP" which is the Access Point mode)
  WiFi.hostname("EchoSensor");      // Hostname to uniquely identify our device
  WiFi.begin(ssid, password);       // Attempts to connect to wifi using provided credentials

  while (WiFi.status() != WL_CONNECTED) {     // While the wifi is not connected yet:
    delay(500);                     // every half second,
    Serial.print(".");              // it prints a dot on the Serial Monitor to show it is still trying to connect
  }
  Serial.println();
  Serial.println("WiFi connected"); // When it does connect, show that there was success

  connectMQTT();                    // Runs the function to set up our MQTT connection
  
  // Blink LED quickly to show that it is connected
  digitalWrite(LED_BUILTIN, LOW);   // light turns on
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);  // light turns off
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);   // light turns on
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);  // light turns off
  
  client.publish("device/echoSensor", "Activated");  // Post in the device channel that this device is on
  
}




 
void loop() {                       // This constantly cycles through, performing MQTT tasks and if the door switch is connected, finding sensor distances and if a range change occurs, posting this to the server
  client.loop();                    // Runs through the MQTT functions to process incoming messages and send publish data

  if (!client.connected()){         // Occasionally the client drops connection randomly and stops working
    Serial.println("MQTT was Unexpectedly Disconnected");
    connectMQTT();                  // If the connection dropped, try to reconnect to our server
  }

  if (startSensor == 1){            // If the door sensor is connected, we can begin to collect echo sensor readings
    digitalWrite(LED_BUILTIN, LOW); // Turn on the LED light to show we are allowed to gather data
    
    // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
    // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
   
    // Read the signal from the sensor: a HIGH pulse whose
    // duration is the time (in ms) from the sending
    // of the ping to the reception of its echo off of an object.
    pinMode(echoPin, INPUT);
    duration = pulseIn(echoPin, HIGH);
   
    // Convert the time into a distance (because the sound is bounced, the total duration is divided by two to gain the distance
    cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343 to obtain cm from ms
    inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135 to obtain in from ms
  
    // Show the reading of inches and centimeters in the Serial Monitor.
    Serial.print(inches);
    Serial.print("in, ");
    Serial.print(cm);
    Serial.print("cm");
    Serial.println();


    // Respond to the readings from the echo sensor. Change the color variable to represent the range of the distance
    if (cm > 200) {
      // This is likely an odd reading, do nothing... (if a room is bigger, expand this number)
    } else if (cm > 35){
      // Distance is over 35cm, a fair bit away. It is safe to keep driving
      color = 'g';
    } else if (cm > 20) {
      // Distance is between 20-35cm, we are getting closer so show a warning light
      color = 'y';
    } else if (cm > 10) {
      // Distance is between 10-20cm, the car should stop now
      color = 'r';
    } else if (cm > 0) {
      // Distance is less than 10ms, show that the driver is too close and there is no walkway and they should back up
      color = 'b';
    }
  } else {                        // The echo sensor is not authorized to work (the magnet switch is disconnected)
    digitalWrite(LED_BUILTIN, HIGH); // The LED light turns off
    color = 'o';                  // The stoplight state should be off because the door is closed
  }

  // If there is a state change (the color range now is different than the last), post the new value to the MQTT server
  if (!(color == lastColor)) {    // The color is different
    if (color == 'g') {
      client.publish("garage/echoSwitch/color", "g");
    } else if (color == 'y') {
      client.publish("garage/echoSwitch/color", "y");
    } else if (color == 'r') {
      client.publish("garage/echoSwitch/color", "r");
    } else if (color == 'b') {
      client.publish("garage/echoSwitch/color", "b");
    } else if (color == 'o') {
      client.publish("garage/echoSwitch/color", "o");
    }
    Serial.println(color);        // Show the new color value in the serial monitor
    lastColor = color;            // Recognize the new color as the last color
  }
  
  // Wait a tenth of a second before taking another sensor reading (this number is variablle, smaller amounts will occur faster)
  delay(100);
}





void connectMQTT() {              // When first connecting to MQTT or when it becomes disconnected, run this script
  client.setServer(mqttServer, mqttPort);     // Setup the server connection with information the user provided
  client.setCallback(callback);   // Setup the callback to listen to changes made on the MQTT server
  while(!client.connected()) {    // If the MQTT server is not connected or becomes disconnected, try again
    Serial.println("connecting to MQTT...");
    if (client.connect("EchoSensor")) {       // Try to connect with this name
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("failed... Re");           // Repeatedly says "failed... Reconnecting" as this loop cycles through
      // Serial.print(client.state());           // Show in the Serial Monitor what the current state of the server connection is
      delay(2000);                // Wait two seconds before trying to connect to the server again
    }
  } 
  client.subscribe("garage/doorSwitch");      // Listen to this path on the server and when a change is noticed, run the callback function
}





void callback(char* topic, byte* payload, unsigned int length)
{
  // The callback listens to the MQTT server and checks for changes in the "garage/doorSwitch"
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
  startSensor = (value.toInt());  // If a one came through, then the loop() function can begin collecting sensor data. If a 0 came through, it will stop
  Serial.println();
}
