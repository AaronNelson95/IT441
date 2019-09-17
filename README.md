# IT441
Projects for the Embedded Computer Systems Fall 2019 Class at Brigham Young University.

## Lab 1
### Wifi-Controlled LED Stoplight
This code in this folder should be placed in the Apache2 web server running on a Raspberry Pi. The Pi will host a php-based webpage that allows the user to select what color they want the attached GPIO traffic light LED display module to light up to. This assumes that the green light is connected to GPIO2 (physical pin 3), yellow is connected to GPIO3 (physical pin 5), and red is connected to GPIO4 (physical pin 7). The ground light module can be connected to any GND on the Pi (such as physical pin 9). 

When the user selects a light color on the hosted webpage, JavaScript is used to highlight that cell on the page's table. While this happens on the client's computer, JavaScript also sends an Ajax XMLHttpRequest to the server, to run a php script there. These simple php scripts turns on the associated light's GPIO pin number, and turns off the others by using the terminal system command `$ gpio -g write 2 1` (where '2' is the GPIO pin number, in this case green, and the final '1' means to turn the value on).
