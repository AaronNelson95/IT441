/*
 * Created by Aaron Nelson
 * Final Project - Arduino Alarm - Weather Screen
 * 12-9-19
 * This script is to be used with an Arduino OLED Shield, screen size 64x48px. It connects to the Adafruit MQTT server and listens to the alarm feed. When it obtains a value of '1', the alarm will activate. This will pull HTTP information from a weather API, parse through its JSON, and show information on a OLED screen.
 * 
 * 
 * The Adafruit.io portion of this script was obtained from the Arduino example (after adding the Adafruit Library) at File -> Examples -> Adafruit MQTT Library -> mqtt_esp8266_callback
 * 
 * The weather API is obtained from https://openweathermap.org/current
 * 
 * The HTTP GET request code was obtained from https://techtutorialsx.com/2016/07/17/esp8266-http-get-requests/
 * 
 * The JSON reader was obtained from https://arduinojson.org/v6/example/http-client/ and Size and parameters were generated from https://arduinojson.org/v6/assistant/
 * 
 * The LED screen portion was obtained from the Arduino example (after adding the Adafruit GFX and Adafruit SSD1306 Wemos Mini OLED libraries) at File -> Examples -> Adafruit SSD1306 Wemos Mini OLED -> ssd1306_64x48_i2c
 *
    OLED Shield pins (if soldering does not allow direct placement):
        3V3: 3V3
        D1 (SCL): D1
        D2 (SDA): D2
        GND: GND
 */

#include <ESP8266WiFi.h>            // This contains the libraries that allows the board to connect to wifi
#include <ESP8266HTTPClient.h>      // Library to perform a HTTP GET request
#include <ArduinoJson.h>            // Library to read and process JSON objects
#include <Wire.h>                   // Used to communicate with I2C devices
#include <Adafruit_GFX.h>           // The Adafruit graphics core library to work with OLED displays
#include <Adafruit_SSD1306.h>       // Library that works with the OLED Shield displays
#include "Adafruit_MQTT.h"          // Library to access data from an Adafruit.io online feed
#include "Adafruit_MQTT_Client.h"   // Used to subscribe to an Adafruit.io online feed


/************************* WiFi Access Point *********************************/
const char* ssid     = "ENTER WIFI HERE";       // Specify the name of your wifi
const char* password = "ENTER PASSWORD HERE";   // Specify the password for your wifi


/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER      "io.adafruit.com"       // Pulling data from the Adafruit website
#define AIO_SERVERPORT  1883                    // use 8883 for SSL
#define AIO_USERNAME    "ENTER ADAFRUIT NAME"   // Username for Adafruit (goes before the /feed/# MQTT feed)
#define AIO_KEY         "ENTER ADAFRUIT KEY"  	// Obtained by going to io.adafruit.com and clicking AIO Key link in top right. Copy the "Active Key" here


/*********************** Weather API Settings *******************************/
String zipCode = "55555";
String weatherAPIKey = "ENTER API KEY";  		// API key from https://openweathermap.org/current


/****************************** Feeds ***************************************/
// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Watch a feed called 'alarm' and subscribe to changes.
Adafruit_MQTT_Subscribe alarm = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Alarm");


/********************** Weather JSON Variables ******************************/
String weatherInformation = "";     // Variable to hold API JSON data
String temperature = "";            // Variable to hold the obtained temperature
String tempMin = "";                // Variable to hold the obtained min temperature 
String tempMax = "";                // Variable to hold the obtained max temperature
String conditions = "";             // Variable to hold weather condition information


/********************** OLED display settings *******************************/
// This defines important variables for the OLED screen to work. If using a different screen size, obtain this information from the corresponding examples
#define OLED_RESET -1  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);
#define XPOS 0
#define YPOS 1
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
#if (SSD1306_LCDHEIGHT != 48)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif





void setup() {
  Serial.begin(115200);             // This allows serial information when connected to a computer

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize the OLED display with the I2C addr 0x3C (for the 64x48)
  // init done

  // Show image buffer on the display hardware. Since the buffer is initialized with an Adafruit splashscreen internally, this will display the splashscreen.
  // display.display();
  
  // Clear the buffer.
  display.clearDisplay();           // Show nothing on the OLED screen
  display.setTextSize(1);           // Set the text size for text (1 is appropriate as the screen has a small resolution)
  display.setTextColor(WHITE);      // Set the text color to white
  display.setCursor(0,0);           // Reset the text placement position to the top left
  display.println("Now");           // Write a message to the screen
  display.println("Turning");
  display.println("On...");
  display.display();                // Actually commit and show this message onto the display screen
  

  /* Connect to WiFi */
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");   // This is shown on the serial if connected to a computer
  Serial.println(ssid);             // It displays the wifi it is trying to connect to

  WiFi.mode(WIFI_STA);              // It sets the wifis mode to "Station" (rather than "AP" which is the Access Point mode)
  WiFi.hostname("Weather Monitor");     // Hostname to uniquely identify our device
  WiFi.begin(ssid, password);       // Attempts to connect to wifi using the provided credentials

  while (WiFi.status() != WL_CONNECTED) {   // While the wifi is not connected yet:
    delay(500);                     // every half second,
    Serial.print(".");              // it prints a dot on the Serial Monitor to show it is still trying to connect
  }
  Serial.println("");
  Serial.println("WiFi connected"); // When it does connect, show that there was success

  mqtt.subscribe(&alarm);           // Setup MQTT subscription for alarm feed
  
  display.clearDisplay();           // Show nothing on the OLED display now that the device has been set up
  display.display();                // Reflect this on the screen
}





void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &alarm) {
      // If something came through the "alarm" feed in Adafruit.io
      Serial.print(F("Got: "));
      String information = "";        // A string to insert the characters read by the feed
      char* ch = (char *)alarm.lastread;
      information.concat(ch);         // Add the last read character to the information string to get the full value
      Serial.println(information);    // Show the value that came in to the Serial Monitor

      // A "1" was received (the alarm is on)
      if (information == "1") {
        Serial.println("Starting the alarm!");
          getWeather();               // Run the function to perform a HTTP GET request
          readJson();                 // Run the function to interpret the received JSON file
          showScreen();               // Show parsed information on the OLED display
      }

      // A "0" was received (the alarm was turned off)
      if (information == "0") {
        Serial.println("Ending the alarm!");
        display.clearDisplay();       // Show nothing on the display
        display.display();            // Commit the plan to clear the screen
      }

      // The word "Weather" was received
      if (information == "Weather") {
        // This will temporarily show the weather information on the screen, only for 10 seconds
        Serial.println("Showing Weather Information");
          // Perform the same functions as when the alarm is on and running
          getWeather();
          readJson();
          showScreen();
          // Then wait for 10 seconds and turn off the display again
          delay(10000);
          display.clearDisplay();
          display.display();
      }
    }
  }
}





// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care of connecting and reconnecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to Adafruit... ");

  uint8_t retries = 3;                  // It will attempt to connect 3 times before predicting the internet is down
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying Adafruit connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;    // Count against one of the retries because there was no success
       if (retries == 0) {
         // basically die and wait for the board to be reset after showing an error message on the display screen
         display.clearDisplay();        // Clear the previous contents of the screen
         display.setCursor(0,0);        // Reset the text cursor to the top left
         display.println("ERROR");      // Display there was an error
         display.println("with MQTT!");
         display.display();             // Commit this to the screen
         while (1);
       }
  }
  Serial.println("Adafruit Connected!");
}





// Function to perform an HTTP Get request on a weather API website to obtain JSON data
void getWeather() {
  // Weather API obtained from https://openweathermap.org/current
  // Template from https://techtutorialsx.com/2016/07/17/esp8266-http-get-requests/
  Serial.println("Obtaining Weather Information... ");  

  weatherInformation = "";              // Variable to hold our weather JSON (clear it if something was in it)
  if (WiFi.status() == WL_CONNECTED) {  //Check WiFi connection status
    HTTPClient http;                    //Declare an object of class HTTPClient
    http.begin("http://api.openweathermap.org/data/2.5/forecast?zip=" + zipCode + ",us&cnt=1&units=imperial&appid=" + weatherAPIKey);      //Specify request destination (the weather API with the user's ZIP and Key)
    int httpCode = http.GET();          // Send the request
    if (httpCode > 0) {                 //Check the returning code
      String payload = http.getString();    //Get the request response payload
      // Serial.println(payload);       //Print the response payload
      weatherInformation.concat(payload);   // Write the response payload to the weatherInformation variable
    }
    http.end();                         //Close the HTTP connection  
  } 
  Serial.println(weatherInformation);   // Show the weather JSON information on the Serial Monitor
}





// Function to parse through obtained JSON data
void readJson() {
  // Example provided at https://arduinojson.org/v6/example/http-client/
  // Size and parameters generated from https://arduinojson.org/v6/assistant/
  Serial.println("Parsing JSON Information... ");
  
  const size_t capacity = 2*JSON_ARRAY_SIZE(1) + 2*JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(7) + JSON_OBJECT_SIZE(8) + 320;   // Find this value online
  DynamicJsonDocument doc(capacity);    // You must specify the JSON document's size to allocate its memory pool in the heap 

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, weatherInformation);
  if (error) {
    // Could not read JSON information
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }


  // Extract and display values, the location and how to obtain these values are also obtained at the ArdunoJSON website
  JsonObject list_0 = doc["list"][0];  
  JsonObject list_0_main = list_0["main"];
  float list_0_main_temp = list_0_main["temp"]; // 28.53
  float list_0_main_temp_min = list_0_main["temp_min"]; // 21.36
  float list_0_main_temp_max = list_0_main["temp_max"]; // 28.53

  JsonObject list_0_weather_0 = list_0["weather"][0];
  const char* list_0_weather_0_main = list_0_weather_0["main"]; // "Clouds"
  const char* list_0_weather_0_description = list_0_weather_0["description"]; // "few clouds"

  // Set the extracted values into our defined variables (also obtained from the website)
  temperature = String(list_0_main_temp);
  tempMin = String(list_0_main_temp_min);
  tempMax = String(list_0_main_temp_max);
  conditions = String(list_0_weather_0_main);

  // Show this content on the serial monitor
  Serial.println(temperature);
  Serial.println(tempMin + "-" + tempMax);
  Serial.println(conditions); 
}





// Function to write extracted JSON variables on the OLED display
void showScreen() {
  // From the example code ssd1306_64x48_i2c
  
  display.clearDisplay();           // Show nothing on the OLED screen
  display.setCursor(0,0);           // Reset the text cursor to the top left
  display.println("Morning!");      // Write out the temperature messages
  display.println("temp:" + temperature);
  display.println("min: " + tempMin);
  display.println("max: " + tempMax);
  display.println(conditions);
  display.display();                // Commit this text to the board
}
