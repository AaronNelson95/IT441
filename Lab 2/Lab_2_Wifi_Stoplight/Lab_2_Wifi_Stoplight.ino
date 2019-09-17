/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
*/

#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "YOUR SSID HERE"
#define STAPSK  "YOUR WIFI PASS HERE"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

unsigned long beginningMillis;
int autoModeNumber;
const unsigned long period = 1000;

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);


  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));

  // Start the server
  server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.println(WiFi.localIP());

  // prepare LED
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(12, OUTPUT); // Pin D6
  pinMode(13, OUTPUT); // Pin D7
  pinMode(15, OUTPUT); // Pin D8
  digitalWrite(LED_BUILTIN, 1);
  digitalWrite(12, 0);
  digitalWrite(13, 0);
  digitalWrite(15, 0);
  autoModeNumber = 0;
  beginningMillis = millis();
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  //Serial.println(F("new client"));

  client.setTimeout(5000); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  //Serial.println(F("request: "));
  //Serial.println(req);



  // Match the request
  String ModeOn = "off";
  if (req.indexOf(F("/off")) != -1) {
    ModeOn = "off";
    autoModeNumber = 0;
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
    digitalWrite(15, LOW);
    Serial.println("Lights Off");
  } else if (req.indexOf(F("/green")) != -1) {
    ModeOn = "green";
    // Green Light
    autoModeNumber = 0;
    digitalWrite(12, HIGH);
    digitalWrite(13, LOW);
    digitalWrite(15, LOW);
    Serial.println("Green Light");
  } else if (req.indexOf(F("/yellow")) != -1) {
    ModeOn = "yellow";
    // Yellow Light
    autoModeNumber = 0;
    digitalWrite(12, LOW);
    digitalWrite(13, HIGH);
    digitalWrite(15, LOW);
    Serial.println("Yellow Light");
  } else if (req.indexOf(F("/red")) != -1) {
    ModeOn = "red";
    // Red Light
    autoModeNumber = 0;
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
    digitalWrite(15, HIGH);
    Serial.println("Red Light");
  } else if (req.indexOf(F("/auto")) != -1) {
    ModeOn = "in auto mode";
    if (autoModeNumber == 0){
      beginningMillis = millis();
    }
    autoModeNumber = 1;
    Serial.println(" - Auto Mode Activated - ");
  } else {
    Serial.println(F("invalid request"));
  }


  // Check out auto mode information
  if (autoModeNumber == 1){
    // should be green
    digitalWrite(12, HIGH);
    digitalWrite(13, LOW);
    digitalWrite(15, LOW);
    Serial.println("Auto Green Light");
  } else if (autoModeNumber == 2){
    // should be yellow
    digitalWrite(12, LOW);
    digitalWrite(13, HIGH);
    digitalWrite(15, LOW);
    Serial.println("Auto Yellow Light");
  } else if (autoModeNumber == 3){
    // should be red
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
    digitalWrite(15, HIGH);
    Serial.println("Auto Red Light");
  }

  if (autoModeNumber > 0){
    if (millis() - beginningMillis > period){
      autoModeNumber = autoModeNumber + 1;
      if (autoModeNumber == 4) {
        autoModeNumber = 1;
      }
      beginningMillis = millis();
    }
  }



  // read/ignore the rest of the request
  // do not client.flush(): it is for output only, see below
  while (client.available()) {
    // byte by byte is not very efficient
    client.read();
  }


  // Send the response to the client
  // it is OK for multiple small client.print/write,
  // because nagle algorithm will group them into one single packet
  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nThe stoplight is now "));
  client.print(ModeOn);
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

  // The client will actually be *flushed* then disconnected
  // when the function returns and 'client' object is destroyed (out-of-scope)
  // flush = ensure written data are received by the other side
  //Serial.println(F("Disconnecting from client"));
}
