/*
 * Aaron Nelson
 * Lab 2 Stoplight Control
 * 9-25-19
 */
#include <ESP8266WiFi.h>            // This contains the libraries that allows the board to connect to wifi

/* The basic server setup and framework was obtained from the Arduino Examples- 
    File -> Examples -> ESP8266WiFI -> WiFiManualWebServer */
#ifndef STASSID
#define STASSID "YOUR SSID HERE"     // Specify the name of your wifi
#define STAPSK  "YOUR PASSWORD HERE"        // Specify the password for that wifi
#endif
const char* ssid = STASSID;
const char* password = STAPSK;

WiFiServer server(80);              // This sets up the server to listen on port 80 (http)

unsigned long beginningMillis;      // A variable to hold our current time (used for auto mode to keep track of how much time has passed)
int autoModeNumber;                 // A variable to keep track of what color the light is in during auto mode

int green = 12;                     // Pin D6 is used for green. If using a different pin, specify that here
int yellow = 13;                    // Pin D7 is used for yellow. If using a different pin, specify that here
int red = 15;                       // Pin D8 is used for red. If using a different pin, specify that here
String currentMode = "off";         // CurrentMode is used to inform the user with visible text what light is on. It also highlights the table cell containing the mode





void setup() {                      // This runs when the board is first turned on
  Serial.begin(115200);             // This allows serial information when connected to a computer (in here, this just tells what light was requested to turn on)
  pinMode(green, OUTPUT);           // Prepares the pin connected to the green light for output
  pinMode(yellow, OUTPUT);          // Prepares the pin connected to the yellow light for output
  pinMode(red, OUTPUT);             // Prepares the pin connected to the red light for output
  autoModeNumber = 0;               // Sets the auto mode number to 0 (meaning it is off and not supposed to cycle)
  lightsOff();                      // Runs the function (at the bottom of this script) that turns all pin lights to LOW/OFF

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");   // This is shown on the serial if connected to a computer
  Serial.print(ssid);               // It displays the wifi it is trying to connect to

  WiFi.mode(WIFI_STA);              // It sets the wifis mode to "Station" (rather than "AP" which is the Access Point mode)
  WiFi.begin(ssid, password);       // Attempts to connect to wifi using provided credentials

  while (WiFi.status() != WL_CONNECTED) {     // While the wifi is not connected yet:
    delay(500);                     // every half second,
    Serial.print(".");              // it shows a dot on the Serial Monitor to show it is still trying to connect
  }
  Serial.println();
  Serial.println("WiFi connected"); // When it does connect, show that there was success

  // Start the server
  server.begin();
  Serial.println("Server started on ");
  Serial.println(WiFi.localIP());   // Show the IP address the board has now that it is connected and broadcasting

}





void loop() {                       // This constantly cycles through, accepting clients, showing them a webpage, and responding to their inputs
  // Check if a client is connected
  WiFiClient client = server.available();     // A client object is created for connections
  if (client) {                     // This waits until a client sends some data (such as by clicking a light option)
    while (!client.available()) {   // The client hasn't sent info so wait for it to do so
      delay(10);
    }
  }
  client.setTimeout(100);           // This sets the maximum ms to wait for data to come through. Each page the client will go has a short url, so a small amount is fine 

  String req = client.readStringUntil('\r');  // Reads the first line of the request (the URL string) made by the client
  if (req.indexOf("/off") != -1) {  // If the request is for the "/off" page:
    autoModeNumber = 0;             // Turn off auto mode
    lightsOff();                    // This function turns off all of the lights                
    currentMode = "off";            // Show that the current state is off to the client
  }
  if (req.indexOf("/green") != -1) {          // If the request is for the "/green" page:
    autoModeNumber = 0;             // Turn off auto mode
    Serial.println("Green Light");  // Print to the computer Serial Monitor that the light is green
    greenLight();                   // This function turns on only the green light
    currentMode = "green";          // Show that the current state is green to the client
  }
  if (req.indexOf("/yellow") != -1) {         // If the request is for the "/yellow" page:
    autoModeNumber = 0;             // Turn off auto mode
    Serial.println("Yellow Light"); // Print to the computer Serial Monitor that the light is yellow
    yellowLight();                  // This function turns on only the yellow light
    currentMode = "yellow";         // Show that the current state is yellow to the client
  }
  if (req.indexOf("/red") != -1) {  // If the request is for the "/red" page:
    autoModeNumber = 0;             // Turn off auto mode
    Serial.println("Red Light");    // Print to the computer Serial Monitor that the light is red
    redLight();                     // This function turns on only the red light
    currentMode = "red";            // Show that the current state is red to the client
  }
  if (req.indexOf("/auto") != -1) {           // If the request is for the "/auto" page:
    autoModeNumber = autoModeNumber + 1;      // Increment the auto number. If it was off (0) turn it on to 1 (green)
    if (autoModeNumber == 4) {      // If the light has cycled through and was in the red state:
      autoModeNumber = 1;           // Set it back to the green state
    }
    Serial.println("Auto Mode Activated");    // Print to the computer Serial Monitor that the lights are now in auto mode
    currentMode = "in auto mode";   // Show that the current state is in auto mode to the client
    beginningMillis = millis();     // Take the current time that the processor has been on and set that as the beginning amount (which will be compared to see how much time passes)
  }




  if (autoModeNumber == 1) {        // The first cycle of the auto mode is to show the green light
    greenLight();                   // Call the green function
  }
  if (autoModeNumber == 2) {        // The second cycle of the auto mode is to show the yellow light
    yellowLight();                  // Call the yellow function
  }
  if (autoModeNumber == 3) {        // The third cycle of the auto mode is to show the red light
    redLight();                     // Call the red function
  }

  if (autoModeNumber > 0) {         // Auto mode is currently on so check the following:
    if ((1000 + beginningMillis) < millis()) {  // If there has been 1 second passed between now and the last light change:
      autoModeNumber = autoModeNumber + 1;      // Increment the color mode that auto mode is currently on
      if (autoModeNumber == 4) {    // If we were just in the red mode,
        autoModeNumber = 1;         // Set the next mode back to green
      }
      beginningMillis = millis();   // This "resets" our timer to be the new starting point for the next period
      delay(5);                     // A short delay so the script processes the changes
    }
  }





  /* This is the webpage that is hosted on port 80 (HTTP) and is shown to the client */
  /* Thanks to Jonathan Black for writing and providing a script that converts HTML to the Arduino Client.Print format */
  client.println("<!doctype html>");
  client.println("<html>");
  client.println("  <head>");
  client.println("    <!--Aaron Nelson");
  client.println("    Last Updated: 9-25-19 -->");
  client.println("    <title>Stoplight</title>");
  // Styles are embedded and specified within the HTML page
  client.println("    <style>");                          
  client.println("      /* UNIVERSAL STYLES */");
  client.println("      /* Overall Page Structure */");
  client.println("      body {");
  client.println("        background-color: #a4b9d5; ");
  client.println("        color:#000000; ");
  client.println("        font-family: Verdana, Arial, serif; ");
  client.println("      }");
  client.println("      #wrapper {");
  client.println("        background-color: #F5F5F5; ");
  client.println("        width: 90%; ");
  client.println("        margin-top: 25px;");
  client.println("        margin-left: auto; ");
  client.println("        margin-right: auto; ");
  client.println("        min-width: 600px; ");
  client.println("        max-width: 1480px; ");
  client.println("      }");
  client.println("");
  client.println("");
  client.println("      /* Header Appearance (Site Title) */");
  client.println("      header {");
  client.println("        text-align: left;");
  client.println("        background-color: #4972aa;");
  client.println("        height: 100px;");
  client.println("      }");
  client.println("      header h1 {");
  client.println("        padding-left: 8%;");
  client.println("        padding-top: 0.6em;");
  client.println("        color: white;");
  client.println("        font-size: 2.5em;");
  client.println("      }");
  client.println("");
  client.println("");
  client.println("      #pageTitle {");
  client.println("        font-size: 1.8em;");
  client.println("        font-style: bold;");
  client.println("        background-color: #4972aa;");
  client.println("        color: white;");
  client.println("        padding: 0.3em;");
  client.println("        text-align: center;");
  client.println("      }");
  client.println("      #main {");
  client.println("        padding-left: 2em;");
  client.println("        padding-right: 2em;");
  client.println("        display: block;");
  client.println("      }");
  client.println("");
  client.println("");
  client.println("      /* Link Colors */");
  client.println("      a:link {color: #000000; text-decoration: none; }");
  client.println("      a:visited {color: #000000; text-decoration: none; }");
  client.println("      a:active {color: #000000; text-decoration: none; }");
  client.println("");
  client.println("");
  client.println("      /* Footer Format */");
  client.println("      footer {");
  client.println("        text-align:center;");
  client.println("        background-color: #a4b9d5;");
  client.println("        font-size: .55em;");
  client.println("        padding: 1em; ");
  client.println("      }");
  client.println("");
  client.println("");
  client.println("");
  client.println("");
  client.println("      /* SPECIFIC WEBPAGE STYELS */");
  client.println("      /* LAB 2 */");
  client.println("      /* Spotlight Table */");
  client.println("      .SpotlightColors {");
  client.println("        width: 100%; ");
  client.println("        border: 0; ");
  client.println("        border-collapse: collapse; ");
  client.println("        margin-bottom: 3em; ");
  client.println("      }");
  client.println("      .SpotlightColors td, th {");
  client.println("        border: 0; ");
  client.println("        padding: 5px; ");
  client.println("        text-align: center; ");
  client.println("      }");
  client.println("      h4 {");
  client.println("        margin-top: 3px;");
  client.println("        margin-bottom: 3px;");
  client.println("      }");
  client.println("      /* Spotlight Table Text */");
  client.println("      .green {");
  client.println("        color: green;");
  client.println("      }");
  client.println("      .yellow {");
  client.println("        color: yellow;");
  client.println("        /* Yellow text is not readable so it needs a black outline*/");
  client.println("        text-shadow:");
  client.println("          -1px -1px 0 #000,");
  client.println("          1px -1px 0 #000,");
  client.println("          -1px 1px 0 #000,");
  client.println("          1px 1px 0 #000;");
  client.println("      }");
  client.println("      .red {");
  client.println("        color: red;");
  client.println("      }");
  client.println("      .auto, .off {");
  client.println("        text-align: center;");
  client.println("      }");
  client.println("      #green:hover, #yellow:hover, #red:hover, #auto:hover, #off:hover {");
  client.println("        /* Change the color of what is being selected */");
  client.println("        background-color: #ddd;");
  client.println("      }");
  client.println("      .running {");
  client.println("        background-color: #a4b9d5;");
  client.println("      }");
  client.println("      .autoOn {");
  client.println("        background-color: #a4b9d5;");
  client.println("      }");
  client.println("      .colorPick {");
  client.println("        width: 50px;");
  client.println("        text-align: right;");
  client.println("      }");
  client.println("          ");
  client.println("    ");
  client.println("    </style>    ");
  // End of the styles and beginning of the HTML page
  client.println("  </head>");
  client.println("  <body>");
  client.println("    <div id=\"wrapper\">");
  client.println("      <header>");
  client.println("        <h1>Aaron's Class Projects</h1>");
  client.println("      </header>");
  client.println("      <div id=\"pageTitle\">Lab 2 - Stoplight Control</div>");
  client.println("    ");
  client.println("      <div id=\"main\"> ");
  client.println("        <p>Choose the color of the stoplight or select \"Auto\" to have it cycle through each light. The light is currently ");
  // This will show in plain text what mode the light is set to show
  client.print(currentMode);
  client.println(".</p>");
  client.println("        <table class=SpotlightColors>");
  client.println("          <tr>");
  client.print("            <td id=\"off\" colspan=3 ");
  // If the current mode is off, the table cells are highlighted with CSS by giving it a special class
  if (currentMode == "off"){
    client.print("class=\"running\" ");
  }

  client.print("><a href='http://");
  // This hyperlink will include the current IP address the server is on
  client.print(WiFi.localIP());
  client.print("/off'>");
  
  client.println("<h4 class=\"off\">OFF</h4></a></td>");
  client.println("          </tr>");
  client.println("          <tr>");
  client.println("            <!-- Stoplight images from https://www.freemanhealth.com/blog/the-behavior-traffic-light-colors-of-the-light -->");
  client.print("            <td id=\"green\" ");
  // If the current mode is green, the table cells are highlighted with CSS by giving it a special class
  if (currentMode == "green"){
    client.print("class=\"running\" ");
  }
  
  client.print("><a href='http://");
  // This hyperlink will include the current IP address the server is on
  client.print(WiFi.localIP());
  client.print("/green'>");
  
  client.println("<img src=\"https://raw.githubusercontent.com/AaronNelson95/IT441/master/Lab%201/Images/GreenLight.png\"><br><h4 class=\"green\">Green</h4></a></td>");
  client.println("");
  client.print("            <td id=\"yellow\" ");
  // If the current mode is yellow, the table cells are highlighted with CSS by giving it a special class
  if (currentMode == "yellow"){
    client.print("class=\"running\" ");
  }
  
  client.print("><a href='http://");
  // This hyperlink will include the current IP address the server is on
  client.print(WiFi.localIP());
  client.print("/yellow'>");
    
  client.println("<img src=\"https://raw.githubusercontent.com/AaronNelson95/IT441/master/Lab%201/Images/YellowLight.png\"><br><h4 class=\"yellow\">Yellow</h4></a></td>");
  client.println("");
  client.print("            <td id=\"red\" ");
  // If the current mode is red, the table cells are highlighted with CSS by giving it a special class
  if (currentMode == "red"){
    client.print("class=\"running\" ");
  }
  client.print("><a href='http://");
  // This hyperlink will include the current IP address the server is on
  client.print(WiFi.localIP());
  client.print("/red'>");
  
  client.println("<img src=\"https://raw.githubusercontent.com/AaronNelson95/IT441/master/Lab%201/Images/RedLight.png\"><br><h4 class=\"red\">Red</h4></a></td>");
  client.println("          </tr>");
  client.println("          <tr>");
  client.print("            <td id=\"auto\" colspan=3 ");
  // If the current mode is on auto, the table cells are highlighted with CSS by giving it a special class
  if (currentMode == "in auto mode"){
    client.print("class=\"running\" ");
  }
  
  client.print("><a href='http://");
  // This hyperlink will include the current IP address the server is on
  client.print(WiFi.localIP());
  client.print("/auto'>");
  
  client.println("<h4 class=\"auto\">Auto</h4></a></td>");
  client.println("          </tr>");
  client.println("        </table>");
  client.println("        ");
  client.println("        ");
  client.println("        ");
  client.println("      ");
  client.println("      </div>");
  client.println("    ");
  client.println("      <footer>Aaron Nelson &emsp; BYU - Embedded Systems - Lab 1 &emsp; Fall 2019");
  client.println("      <br>This page was last modified on: 9/25/2019");
  client.println("      </footer>");
  client.println("    </div>");
  client.println("  </body>");
  client.println("</html>");
}





/* These are the functions that are called when a client sends a request. 
    They only turn on the light they are supposed to according to the pin number specified at the beginning of the script */
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
