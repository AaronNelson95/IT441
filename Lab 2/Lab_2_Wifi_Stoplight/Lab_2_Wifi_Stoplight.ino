/*
 * Aaron Nelson
 * 9-24-19
 */
#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "YOUR SSID HERE"
#define STAPSK  "YOUR PASSWORD HERE"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

WiFiServer server(80);

unsigned long beginningMillis;
int autoModeNumber;

int green = 12; // Pin D6
int yellow = 13;  // Pin D7
int red = 15; // Pin D8
String currentMode = "off";

void setup() {
  Serial.begin(115200);
  pinMode(green, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(red, OUTPUT); 
  autoModeNumber = 0;
  lightsOff();

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started on ");
  Serial.println(WiFi.localIP());

}





void loop() {
  // Check if a client is connected
  WiFiClient client = server.available();
  if (client) {
    while (!client.available()) { // client hasn't sent info so wait for it
      delay(10);
    }
  }
  client.setTimeout(100);

  String req = client.readStringUntil('\r');
  if (req.indexOf("/off") != -1) {
    autoModeNumber = 0;
    lightsOff();
    currentMode = "off";
  }
  if (req.indexOf("/green") != -1) {
    autoModeNumber = 0;
    Serial.println("Green Light");
    greenLight();
    currentMode = "green";
  }
  if (req.indexOf("/yellow") != -1) {
    autoModeNumber = 0;
    Serial.println("Yellow Light");
    yellowLight();
    currentMode = "yellow";
  }
  if (req.indexOf("/red") != -1) {
    autoModeNumber = 0;
    Serial.println("Red Light");
    redLight();
    currentMode = "red";
  }
  if (req.indexOf("/auto") != -1) {
    autoModeNumber = autoModeNumber + 1;
    if (autoModeNumber == 4) {
      autoModeNumber = 1;
    }
    Serial.println(" - Auto Mode Activated - ");
    currentMode = "in auto mode";
    beginningMillis = millis();
  }




  if (autoModeNumber == 1) {
    //Serial.println("Auto Green Light");
    greenLight();
  }
  if (autoModeNumber == 2) {
    //Serial.println("Auto Yellow Light");
    yellowLight();
  }
  if (autoModeNumber == 3) {
    //Serial.println("Auto Red Light");
    redLight();
  }

  if (autoModeNumber > 0) {
    //Serial.println(beginningMillis);
    //Serial.println(millis());
    if ((2000 + beginningMillis) < millis()) {
      autoModeNumber = autoModeNumber + 1;
      if (autoModeNumber == 4) {
        autoModeNumber = 1;
      }
      beginningMillis = millis();
      delay(5);
    }
  }



  // Show the webpage on the browser
  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nThe stoplight is now "));
  client.print(currentMode);
  client.print(F("<br><br>Click <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/off'>here</a> to switch Off.<br> Click <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/green'>here</a> to switch Green.<br> Click <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/yellow'>here</a> to switch Yellow.<br> Click <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/red'>here</a> to switch Red.<br> Click <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/auto'>here</a> to switch the light to auto mode.</html>"));




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
