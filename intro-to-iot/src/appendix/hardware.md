# A1. Hardware Required

If you're taking the class session in person, you can pretty much skip this page. This page is just documentation of the Arduino setup for those who are remote and interested in purchasing the hardware necessary to run the code for this course.

## List of Hardware

These will mostly link to Sparkfun because they're local to me, but many of these items can be found on the usual sites including Adafruit, Digikey, Mouser, etc.

* Arduino UNO R3 (https://www.sparkfun.com/products/11021)
* Breadboard (https://www.sparkfun.com/products/12002)
* Jumper Wires (https://www.sparkfun.com/products/12795)
* AHT20 Breakout Board (https://www.sparkfun.com/products/16618)

You'll want to find a place where you can access a soldering iron in order to solder header pins on the breakout board. Check out your local makerspaces or libraries (which sometimes have makerspaces in them these days).

## Circuit Diagram

The circuit is super simple, thanks to Arduino.

* Arduino 3V3 <--> AHT20 3.3V
* Arduino GND <--> AHT20 GND
* Arduino SDA <--> AHT20 SDA
* Arduino SCL <--> AHT20 SCL

Alternatively, Sparkfun is all in on their "QWIIC" connector (just a 4-pin JST connector). You can get a version of the Arduino UNO R3 with a QWIIC connector onboard and QWIIC cables; this will mean no soldering if you can't find that and don't own an iron already.