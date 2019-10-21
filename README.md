# IT441
Projects for the Embedded Computer Systems Fall 2019 Class at Brigham Young University.

## Lab 1
### Wifi-Controlled LED Stoplight
This code in this folder should be placed in the Apache2 web server running on a Raspberry Pi. The Pi will host a php-based webpage that allows the user to select what color they want the attached GPIO traffic light LED display module to light up to. This assumes that the green light is connected to GPIO2 (physical pin 3), yellow is connected to GPIO3 (physical pin 5), and red is connected to GPIO4 (physical pin 7). The ground light module can be connected to any GND on the Pi (such as physical pin 9). 

When the user selects a light color on the hosted webpage, JavaScript is used to highlight that cell on the page's table. While this happens on the client's computer, JavaScript also sends an Ajax XMLHttpRequest to the server, to run a php script there. These simple php scripts turns on the associated light's GPIO pin number, and turns off the others by using the terminal system command `$ gpio -g write 2 1` (where '2' is the GPIO pin number, in this case green, and the final '1' means to turn the value on).

The public page for this lab is at http://aaronnelson95.com/IT441Lab1.php


## Lab 2
### Wifi-Controlled LED Stoplight (Using Arduino)
This script should be imported into the Arduino IDE and is designed to be uploaded to a WEMOS D1 mini board. Before uploading it, the current wifi SSID and password should be entered in at the top of the script. The pins for the light colors can also be changed, but this assumes the green light is connected to the D6 pin, the yellow light is connected to the D7 pin, the red light is connected to the D8 pin, and the lights ground is connected to the GND pin.

When the board is powered on, it attempts to connect to wifi. If successful, it will host a server and its IP address will be shown on the Arduino IDE's Serial Monitor. Access this IP in a web browser to see a HTML site that will allow a user to select a color the light should change to, or use auto mode, which will cycle through each color.

Arduino shows a web page by using multiple `client.println("");` commands. The PowerShell script located in the Lab 2 folder can easily convert an HTML page to this format. First, edit the .ps1 file to point to your HTML page, then tell it where to output the information. Run it with Windows PowerShell.

The public page for this lab is at http://aaronnelson95.com/IT441Lab2.php


## Lab 3
### Machine to Machine Communication - Vehicle Sensor
This script should be imported into the Arduino IDE and is designed to be uploaded to two WEMOS D1 mini boards. Before uploading it, the current wifi SSID and password should be entered in at the top of the script. After the sensor server is turned on and connected to wifi, enter its IP address into the stoplight client page and upload the script. The pins for the stoplight client light colors can be changed, but this assumes the green light is connected to the D6 pin, the yellow light is connected to the D7 pin, the red light is connected to the D8 pin, and the lights ground is connected to the GND pin. In addition, this assumes that the Ultrasonic Sensor (HC-SR04) has the Vcc connected to 5V on the board, Trig is connected to pin D7, Echo is connected to pin D6, and Gnd is connected to the boards GND.

When the sensor server is connected to wifi, it will begin to detect distances with the ultrasonic sensor. It will then print this value to the board's server on port 17. While this is continually happening, the stoplight client will read the value provided by the server. Depending on the distance provided, a light will be shown to reflect how far away an object is away from the sensor.

The public page for this lab is at http://aaronnelson95.com/IT441Lab3.php 


## Lab 4
### MQTT Event Hub - Garage Door Sensor
Coming Soon


The public page for this lab is at http://aaronnelson95.com/IT441Lab4.php
