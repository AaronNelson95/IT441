/*
 * Created by Aaron Nelson
 * Lab 3 Vehicle Sensor
 * 10-14-19
 * This script is used with the stoplight module to respond to and act as a client when reading data from a server that posts values from an ultrasonic sensor.
 *  
 * The basic client setup and framework was obtained from the Arduino Examples-
 *      File -> Examples -> ESP8266WIFI -> WiFiClient
 *
    Stoplight Pins:
        Green: Pin12 (D6)
        Yellow: Pin13 (D7)
        Red: Pin15 (D8)
        GND: GND
*/

#include <ESP8266WiFi.h>            // This contains the libraries that allows the board to connect to wifi

#ifndef STASSID
#define STASSID "YOUR WIFI HERE"     // ***Specify the name of your wifi
#define STAPSK  "YOUR PASSWORD HERE" // ***Specify the password for that wifi
#endif
const char* ssid = STASSID;
const char* password = STAPSK;

const char* host = "192.168.***.***";    // ***The IP address the server is running on (as specified when the sensor server connect to internet. Enter that IP here)
const uint16_t port = 17;           // The port that the server is running on (as specified in the sensor server)

int green = 12;                     // Pin D6 is used for green. If using a different pin, specify that here
int yellow = 13;                    // Pin D7 is used for yellow. If using a different pin, specify that here
int red = 15;                       // Pin D8 is used for red. If using a different pin, specify that here
int redFlash = 0;                   // While this value is 1, it specifies the light should be red, while 0, it is off during the flashing stage



void setup() {                      // This runs when the board is first turned on
  Serial.begin(115200);             // This allows serial information when connected to a computer (in here, this just tells what cm reading is seen from the server)
  pinMode(green, OUTPUT);           // Prepares the pin connected to the green light for output
  pinMode(yellow, OUTPUT);          // Prepares the pin connected to the yellow light for output
  pinMode(red, OUTPUT);             // Prepares the pin connected to the red light for output

  // We start the client by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");   // This is shown on the serial if connected to a computer
  Serial.println(ssid);             // It displays the wifi it is trying to connect to

  WiFi.mode(WIFI_STA);              // It sets the wifis mode to "Station" (rather than "AP" which is the Access Point mode)
  WiFi.begin(ssid, password);       // Attempts to connect to wifi using provided credentials

  while (WiFi.status() != WL_CONNECTED) {     // While the wifi is not connected yet:
    delay(500);                     // every half second,
    Serial.print(".");              // it prints a dot on the Serial Monitor to show it is still trying to connect
  }
  Serial.println("");
  Serial.println("WiFi connected"); // When it does connect, show that there was success
}





void loop() {                       // This constantly cycles through, checking the server and reflecting the proper stoplight color
  // Use WiFiClient class to create TCP connections
  WiFiClient client;                // A client object is created for connections
  if (!client.connect(host, port)) {          // If we cannot connect to the server,
    Serial.println("connection failed");      // Show this in the Serial Monitor
    delay(5000);                    // Wait before trying to connect again (after loop cycles)
    return;
  }

  // wait for data to be available
  unsigned long timeout = millis();           // Used for a non-blocking loop to check if the client times out
  while (client.available() == 0) {           // No client is available
    if (millis() - timeout > 5000) {          // 5 seconds pass before a client is picked up again
      Serial.println(">>> Client Timeout !");   // Timeout the client
      client.stop();
      delay(30000);                 // Wait half a minute before trying to reconnect. There must be a server problem
      return;
    }
  }

  // Read all the lines of the reply from server and print them to the Serial Monitor
  // Information is read from the server one character at a time. This will add up those characters to make up the number values (from a string)
  String number = "";               // String to receive the web server characters
  while (client.available()) {      // While the server is connected
    char ch = static_cast<char>(client.read()); // Read each character one at a time
    number.concat(ch);              // Add these values to our string
  }
  Serial.println(number.toInt());   // Show the received value on the Serial Monitor
  int value = (number.toInt());     // Turn the received value to an integer to compare
 
  
  // Use the value the sensor sends to change the light color
    if (value > 130) {
      // This is likely an odd reading, do nothing... (if a room is bigger, expand this number)
    } else if (value > 35){
      // Distance is over 35cm, a fair bit away. It is safe to keep driving
      greenLight();
    } else if (value > 20) {
      // Distance is between 20-35cm, we are getting closer so show a warning light
      yellowLight();
    } else if (value > 10) {
      // Distance is between 10-20cm, the car should stop now
      redLight();
    } else if (value > 0) {
      // Distance is less than 10ms, show that the driver is too close and there is no walkway and they should back up
      digitalWrite(green, LOW);     // Don't show the green light if it bounced to this state
      digitalWrite(yellow, LOW);    // Don't show the yellow light if it bounced to this state
      // Bounce between "redFlash" values of 0 or 1 each cycle to flash the red light
      if (redFlash == 0){
        digitalWrite(red, HIGH);    // Turn on the red light
        redFlash = 1;
      } else if (redFlash == 1) {
        digitalWrite(red, LOW);     // Turn off the red (all) light(s)
        redFlash = 0;
      }
      
    }

  client.stop();                    // Close the connection to the client
  delay(150);                       // Wait 150ms before reading the server for data again (this number can be variable depending on how often you'd like to make requests)
}




/* These are the functions that are called after a sensor reading is read. 
    They only turn on the light they are supposed to according to the pin number specified at the beginning of the script */
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
