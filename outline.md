# Course Outline - IoT with Arduino and Python
Draft outline for an introductory course to the Internet of Things with Arduino and Python.

## Goal
### Audience
People who have a basic understanding of programming concepts, electronics knowledge optional.

### Outcome
An Arduino sketch that can sample data from an AHT20 sensor over I2c and report the
data over serial to a python program which sends the data onward via MQTT.

In plain English:
A hardware sensor which can measure the room's temperature and send the data over the internet
to display a historical graph of temperature data.

### Setup for Students
Breadboard with Arduino Nano and i2c-based AHT20 sensor plus USB cable.

### Setup for Me
System on network with MQTT broker and Influx server. Influx server configured with telegraf
plugin to read and tag temperature data from incoming mqtt messages.

### Parts
* Get a blink program running on Arduino.
Get an LED blinking program running so everyone knows how to compile and flash and has correct drivers installed.
* Print LED state into the serial console as it blinks.
Demonstrates serial console in Arduino.
* Get a serial console running in python.
Just prints to the screen in the shell everything that the Arduino was printing to the serial console.
* Start sampling the temperature sensor and printing the value to serial (as ASCII).
Demo the raw ascii in the python program.
* Parse with regex and print the variable in the python program.
Print the data to the terminal (not the ascii data but the raw data)
* Send data over an MQTT topic.
Demo with an Influx dashboard.
