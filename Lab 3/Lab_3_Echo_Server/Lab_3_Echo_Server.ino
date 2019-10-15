/*
 * Created by Aaron Nelson
 * Lab 3 Vehicle Sensor
 * 10-14-19
 * This script is used for the ultrasonic sensor (HC-SR04) to send readings frequently to a server. It works in collaboration with the stoplight, which reads from this server.
 * 
 * Original Script for the sensor functionality was created by Rui Santos, provided at:
 *      https://randomnerdtutorials.com/complete-guide-for-ultrasonic-sensor-hc-sr04/ 
 * 
 * The basic server setup and framework was obtained from the Arduino Examples-
 *      File -> Examples -> ESP8266WIFI -> WiFiManualWebServer
 *
    Ultrasonic Sensor Pins:
        VCC: +5VDC
        Trig: Trigger (INPUT) - Pin13 (D7)
        Echo: Echo (OUTPUT) - Pin12 (D6)
        GND: GND
 */

#include <ESP8266WiFi.h>            // This contains the libraries that allows the board to connect to wifi

#ifndef STASSID
#define STASSID "YOUR WIFI HERE"     // ***Specify the name of your wifi
#define STAPSK  "YOUR PASSWORD HERE" // ***Specify the password for that wifi
#endif
const char* ssid = STASSID;
const char* password = STAPSK;

WiFiServer server(17);              // This sets up the server to listen on port 17

int trigPin = 13;                   // The trigger is on pin D7. If using a different pin, specify that here
int echoPin = 12;                   // The echo is on pin D6. If using a different pin, specify that here
long duration, cm, inches;          // This code can read distances in both cm and inches. They will both be printed to the Serial Monitor


 
void setup() {                      // This runs when the board is first turned on
  Serial.begin (115200);            // This allows serial information when connected to a computer (in here, this shows a constant stream of sensor readings)

  pinMode(trigPin, OUTPUT);         // Trigger acts as an output (it sends off a wave)
  pinMode(echoPin, INPUT);          // Echo acts as in input (it listens for the wave to come back)

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");   // This is shown on the serial if connected to a computer
  Serial.print(ssid);               // It displays the wifi it is trying to connect to

  WiFi.mode(WIFI_STA);              // It sets the wifis mode to "Station" (rather than "AP" which is the Access Point mode)
  WiFi.begin(ssid, password);       // Attempts to connect to wifi using provided credentials

  while (WiFi.status() != WL_CONNECTED) {     // While the wifi is not connected yet:
    delay(500);                     // every half second,
    Serial.print(".");              // it prints a dot on the Serial Monitor to show it is still trying to connect
  }
  Serial.println();
  Serial.println("WiFi connected"); // When it does connect, show that there was success

  // Start the server
  server.begin();
  Serial.println("Server started on ");
  Serial.println(WiFi.localIP());   // Show the IP address the board has now that it is connected and broadcasting

}




 
void loop() {                       // This constantly cycles through, reading value from the sensor and printing that on the hosted server
  WiFiClient client = server.available();     // A client object is created for connections
  
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
 
  // Convert the time into a distance (because the sound is bounced, the total duration is divided by two to gain the correct distance
  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343 to obtain cm from ms
  inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135 to obtain in from ms

  // Show the reading of inches and centimeters in the Serial Monitor.
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

  // Print the number of centimeters on the web server (which is read by the stoplight client)
  client.println(cm);

  // Wait half of a second before taking another sensor reading (this number is variable, smaller amounts will occur faster but you don't want to bog down the server)
  delay(250);
}
