// https://arduinodiy.wordpress.com/2017/11/24/mqtt-for-beginners/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "YOUR SSID";  
const char* password = "YOUR PASSWORD";

const char* mqttServer = "192.168.137.1"; 
const int mqttPort = 1883; 

WiFiClient doorSwitchClient;
PubSubClient client(doorSwitchClient);

int reed_switch = 12;
int reed_status;      // When is 0 it means it is connected to the other device. 1 means it is disconnected
int last_reed_status;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  pinMode(reed_switch, INPUT_PULLUP);       // Pin D6
  reed_status = digitalRead(reed_switch);
  last_reed_status = reed_status;

  /* Connect to WiFi */
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");   // This is shown on the serial if connected to a computer
  Serial.println(ssid);             // It displays the wifi it is trying to connect to

  WiFi.mode(WIFI_STA);              // It sets the wifis mode to "Station" (rather than "AP" which is the Access Point mode)
  WiFi.hostname("DoorSwitch");
  WiFi.begin(ssid, password);       // Attempts to connect to wifi using provided credentials

  while (WiFi.status() != WL_CONNECTED) {     // While the wifi is not connected yet:
    delay(500);                     // every half second,
    Serial.print(".");              // it prints a dot on the Serial Monitor to show it is still trying to connect
  }
  Serial.println("");
  Serial.println("WiFi connected"); // When it does connect, show that there was success

  /* Connect to MQTT */
  connectMQTT();



  // Blink LED quickly to show that it is connected
  digitalWrite(LED_BUILTIN, LOW); // light turns on
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH); // light turns off
  delay(100);
  digitalWrite(LED_BUILTIN, LOW); // light turns on
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH); // light turns off
  
  client.publish("device/doorSwitch", "Activated");
}

void loop() {
  if (!client.connected()){
    Serial.println("MQTT was Unexpectedly Disconnected");
    connectMQTT();
  }
  
  reed_status = digitalRead(reed_switch);

  // Respond to state changes
  if (reed_status == 0 && last_reed_status == 1) {
      digitalWrite(LED_BUILTIN, LOW); // light turns on
      Serial.println("connected");
      client.publish("garage/doorSwitch", "1");
      last_reed_status = 0; 
  }
  if (reed_status == 1 && last_reed_status == 0) {
       digitalWrite(LED_BUILTIN, HIGH); // light turns off
       Serial.println("disconnected");
       client.publish("garage/doorSwitch", "0");
       last_reed_status = 1;
  }
  delay(10);    // done so that the sensor cant be actidentally not reacing it (real quick on and off streams)
}



void connectMQTT() {
  client.setServer(mqttServer, mqttPort);
  while(!client.connected()) {
    Serial.println("connecting to MQTT...");
    if (client.connect("DoorSwitch")) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("failed... Re");
      Serial.print(client.state());
      delay(2000);
    }
  } 
}
