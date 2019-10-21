/*
 * Created by Aaron Nelson
 * Lab 3 Vehicle Sensor
 * 10-14-19
 * This script is used with the stoplight module to respond to and act as a client when reading data from a server that posts values from an ultrasonic sensor.
 *  
 * The basic client setup and framework was obtianed from the Arduino Examples-
 *      File -> Examples -> ESP8266WIFI -> WiFiClient
 *
    Stoplight Pins:
        Green: Pin12 (D6)
        Yellow: Pin13 (D7)
        Red: Pin15 (D8)
        GND: GND
*/

#include <ESP8266WiFi.h>            // This contains the libraries that allows the board to connect to wifi
#include <PubSubClient.h>

const char* ssid = "YOUR WIFI";     // ***Specify the name of your wifi
const char* password = "YOUR PASSWORD";     // ***Specify the password for that wifi

const char* mqttServer = "192.168.137.1"; 
const int mqttPort = 1883; 

unsigned long beginningMillis;      // A variable to hold our current time (used for auto mode to keep track of how much time has passed)
int onBlink;

WiFiClient stoplightClient;
PubSubClient client(stoplightClient);

int green = 12;                     // Pin D6 is used for green. If using a different pin, specify that here
int yellow = 13;                    // Pin D7 is used for yellow. If using a different pin, specify that here
int red = 15;                       // Pin D8 is used for red. If using a different pin, specify that here
int redFlash = 0;                   // While this value is 1, it specifies the light should be red, while 0, it is off during the flashing stage


void setup() {                      // This runs when the board is first turned on
  Serial.begin(115200);             // This allows serial information when connected to a computer (in here, this just tells what cm reading is seen from the server)
  pinMode(green, OUTPUT);           // Prepares the pin connected to the green light for output
  pinMode(yellow, OUTPUT);          // Prepares the pin connected to the yellow light for output
  pinMode(red, OUTPUT);             // Prepares the pin connected to the red light for output
  lightsOff();


  // We start the client by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");   // This is shown on the serial if connected to a computer
  Serial.println(ssid);             // It displays the wifi it is trying to connect to

  WiFi.mode(WIFI_STA);              // It sets the wifis mode to "Station" (rather than "AP" which is the Access Point mode)
  WiFi.hostname("Stoplight");
  WiFi.begin(ssid, password);       // Attempts to connect to wifi using provided credentials

  while (WiFi.status() != WL_CONNECTED) {     // While the wifi is not connected yet:
    delay(500);                     // every half second,
    Serial.print(".");              // it prints a dot on the Serial Monitor to show it is still trying to connect
  }
  Serial.println("");
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
  
  client.publish("device/stoplight", "Activated");
  
}





void loop() {                       // This constantly cycles through, checking the server and reflecting the proper stoplight color
  client.loop();

  if (!client.connected()){
    Serial.println("MQTT was Unexpectedly Disconnected");
    connectMQTT();
  }  

  if (onBlink == 1){
    if ((325 + beginningMillis) < millis()) {
    
      if (redFlash == 0){
          redLight();
          redFlash = 1;
      } else if (redFlash == 1) {
          lightsOff();
          redFlash = 0;
      }
      beginningMillis = millis();
      delay(5);
    }

    
  }
  
}


void connectMQTT() {
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while(!client.connected()) {
    Serial.println("connecting to MQTT...");
    if (client.connect("Stoplight")) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("failed... Re");
      Serial.print(client.state());
      delay(2000);
    }
  } 
  client.subscribe("garage/echoSwitch/color");
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
  Serial.println();




  if (value == "g") {
    greenLight();
    onBlink = 0;
  } else if (value == "y") {
    yellowLight();
    onBlink = 0;
  } else if (value == "r") {
    redLight();
    onBlink = 0;
  } else if (value == "b") {
    lightsOff();
    onBlink = 1;
    beginningMillis = millis();
  } else if (value == "o") {
    lightsOff();
    onBlink = 0;
  }
}




/* These are the functions that are called after a sensor reading is read. 
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
