/*
 * Created by Aaron Nelson
 * Lab 3 Vehicle Sensor
 * 10-14-19
 * This script is used for the ultrasonic sensor (HC-SR04) to send readings frequently to a server. It works in collaboration with the stoplight, which reads from this server.
 * 
 * Original Script for the sensor functionality was created by Rui Santos, provided at:
 *      https://randomnerdtutorials.com/complete-guide-for-ultrasonic-sensor-hc-sr04/ 
 * 
 * The basic server setup and framework was obtianed from the Arduino Examples-
 *      File -> Examples -> ESP8266WIFI -> WiFiManualWebServer
 *
    Ultrasonic Sensor Pins:
        VCC: +5VDC
        Trig: Trigger (INPUT) - Pin13 (D7)
        Echo: Echo (OUTPUT) - Pin12 (D6)
        GND: GND
 */

#include <ESP8266WiFi.h>            // This contains the libraries that allows the board to connect to wifi
#include <PubSubClient.h>

const char* ssid = "YOUR WIFI";     // ***Specify the name of your wifi
const char* password = "YOUR PASSWORD";     // ***Specify the password for that wifi

const char* mqttServer = "192.168.137.1"; 
const int mqttPort = 1883; 



WiFiClient echoClient;
PubSubClient client(echoClient);

int trigPin = 13;                   // The trigger is on pin D7. If using a different pin, specify that here
int echoPin = 12;                   // The echo is on pin D6. If using a differnet pin, specify that here
long duration, cm, inches;          // This code can read distances in both cm and inches. They will both be printed to the Serial Monitor
int startSensor = 0;

char color = 'o';
char lastColor = 'g';

void setup() {                      // This runs when the board is first turned on
  Serial.begin (115200);            // This allows serial information when connected to a computer (in here, this shows a constant stream of sensor readings)
  
  pinMode(trigPin, OUTPUT);         // Trigger acts as an output (it sends off a wave)
  pinMode(echoPin, INPUT);          // Echo acts as in input (it listens for the wave to come back)
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");   // This is shown on the serial if connected to a computer
  Serial.print(ssid);               // It displays the wifi it is trying to connect to

  WiFi.mode(WIFI_STA);              // It sets the wifis mode to "Station" (rather than "AP" which is the Access Point mode)
  WiFi.hostname("EchoSensor");
  WiFi.begin(ssid, password);       // Attempts to connect to wifi using provided credentials

  while (WiFi.status() != WL_CONNECTED) {     // While the wifi is not connected yet:
    delay(500);                     // every half second,
    Serial.print(".");              // it prints a dot on the Serial Monitor to show it is still trying to connect
  }
  Serial.println();
  Serial.println("WiFi connected"); // When it does connect, show that there was success

  connectMQTT();
  
  // Blink LED quickly to show that it is connected
  digitalWrite(LED_BUILTIN, LOW); // light turns on
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH); // light turns off
  delay(100);
  digitalWrite(LED_BUILTIN, LOW); // light turns on
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH); // light turns off
  
  client.publish("device/echoSensor", "Activated");
  
}




 
void loop() {                       // This constantly cycles through, reading value from the sensor and printing that on the hosted server
  client.loop();

  if (!client.connected()){
    Serial.println("MQTT was Unexpectedly Disconnected");
    connectMQTT();
  }



  if (startSensor == 1){
    digitalWrite(LED_BUILTIN, LOW); // light turns on
    
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
   
    // Convert the time into a distance (becuase the sound is bounced, the total duration is divided by two to gain the distance
    cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343 to obtain cm from ms
    inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135 to obtain in from ms
  
    // Show the reading of inches and centemeters in the Serial Monitor.
    Serial.print(inches);
    Serial.print("in, ");
    Serial.print(cm);
    Serial.print("cm");
    Serial.println();




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



    
  
  

  } else {
    digitalWrite(LED_BUILTIN, HIGH); // light turns off
    color = 'o';
  }



  if (!(color == lastColor)) {
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
    Serial.println(color);
    lastColor = color;
  }
  
  // Wait half of a second before taking another sensor reading (this number is variablle, smaller amounts will occur faster but you don't want to bog down the server)
  delay(100);
}




void connectMQTT() {
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while(!client.connected()) {
    Serial.println("connecting to MQTT...");
    if (client.connect("EchoSensor")) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("failed... Re");
      Serial.print(client.state());
      delay(2000);
    }
  } 
  client.subscribe("garage/doorSwitch");
}

void callback(char* topic, byte* payload, unsigned int length)
{

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");

  String value = "";
  for(int i = 0; i < length; i++) {
    char ch = ((char)payload[i]);
    value.concat(ch);
  }
  Serial.print(value);
  startSensor = (value.toInt());
  Serial.println();
}
