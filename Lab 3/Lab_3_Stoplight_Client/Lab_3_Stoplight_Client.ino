/*
    This sketch establishes a TCP connection to a "quote of the day" service.
    It sends a "hello" message, and then prints received data.
*/

#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "YOUR SSID"
#define STAPSK  "YOUR PASSWORD"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "192.168.137.86";
const uint16_t port = 17;


int green = 12;                     // Pin D6 is used for green. If using a different pin, specify that here
int yellow = 13;                    // Pin D7 is used for yellow. If using a different pin, specify that here
int red = 15;                       // Pin D8 is used for red. If using a different pin, specify that here



void setup() {
  Serial.begin(115200);

  pinMode(green, OUTPUT);           // Prepares the pin connected to the green light for output
  pinMode(yellow, OUTPUT);          // Prepares the pin connected to the yellow light for output
  pinMode(red, OUTPUT);             // Prepares the pin connected to the red light for output
  lightsOff();



  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
}

void loop() {
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }

  // wait for data to be available
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      delay(60000);
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  //Serial.println("receiving from remote server");
  // not testing 'client.connected()' since we do not need to send data here
  String number = "";
  while (client.available()) {
    char ch = static_cast<char>(client.read());
    number.concat(ch);
    //Serial.print(ch);
  }
  //Serial.println(number);
  Serial.println(number.toInt());
  // Close the connection
  //Serial.println();
  //Serial.println("closing connection");

  
  int value = (number.toInt());
  if (value > 100) {
    
  } else if (value > 30){
    //Serial.println("green");
    greenLight();
  } else if (value > 15) {
    //Serial.println("yellow");
    yellowLight();
  } else if (value > 0) {
    //Serial.println("red");
    redLight();
  }






  
  client.stop();
  delay(250); // execute once every 5 minutes, don't flood remote service
}



void lightsOff() {
  digitalWrite(green, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(red, LOW);
  Serial.println("Lights Off");
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
