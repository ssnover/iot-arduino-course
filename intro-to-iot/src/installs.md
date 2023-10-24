# Install Prerequisites

This page will walk you through installing all of the tools required for building the code for the Arduino and running the Python script on your host system.

## Arduino IDE

I won't repeat too much information here which could easily go out of date and instead refer to Arduino's documentation for [installing the Arduino IDE](https://support.arduino.cc/hc/en-us/articles/360019833020-Download-and-install-Arduino-IDE). Instructions there are available for Windows, macOS, and Linux.

Verify your installation by opening the IDE, creating a new sketch, and navigating the menu to `Tools` -> `Board` -> `Arduino AVR Boards` -> `Arduino UNO`. This is the board we'll be focusing on in this course. Then select the checkmark button in the top left and verify that the output window eventually finishes printing and shows something like below:

```
Sketch uses 444 bytes (1%) of program storage space. Maximum is 32256 bytes.
Global variables use 9 bytes (0%) of dynamic memory, leaving 2039 bytes for local variables. Maximum is 2048 bytes.
```

### Ubuntu 22.04-based Linux Distros

A quick note that if you're using an unofficial Arduino, you'll find that the device does not actually mount as it's supposed to. If you run `lsusb` you might see an entry for CH340 or CP210x USB drivers, but no device at `/dev/ttyACM0` or `/dev/ttyUSB0`. By default Ubuntu 22.04 and distributions based on it ship with a Braille reader software called `brltty` which is aggressive in claiming devices it thinks are Braille readers.

In `dmesg`, you might see something like this as a symptom:
```
input: BRLTTY 6.4 Linux Screen Driver Keyboard as /devices/virtual/input/input31
usb 1-8: usbfs: interface 0 claimed by ch341 while 'brltty' sets config #1
ch341-uart ttyUSB0: ch341-uart converter now disconnected from ttyUSB0
```

If you are not blind and no one who is blind is using your computer, feel free to uninstall it: `sudo apt remove brltty`. If you want to keep the software on your system for one reason or another, try:
```sh
sudo systemctl stop brltty-udev.service
sudo systemctl mask brltty-udev.service
sudo systemctl stop brltty.service
sudo systemctl disable brltty.service
```

Or if you use a Braille screen reader, you might just try editing the udev rules at `/usr/lib/udev/rules.d/85-brltty.rules`. For the CH340 (with USB VID and PID of `1a86:7523`), remove the corresponding entry:
```
# Device: 1A86:7523
# Baum [NLS eReader Zoomax (20 cells)]
ENV{PRODUCT}=="1a86/7523/*", ENV{BRLTTY_BRAILLE_DRIVER}="bm", GOTO="brltty_usb_run"
```

## Python and pip

Similarly, for installing `python3` and `pip`, there are great directions available. Here is a link to Python's [documentation for installation](https://wiki.python.org/moin/BeginnersGuide/Download) on various operating systems.

Verify your installation in two parts. First, python:
```
python3 --version
```

And then pip:
```
python3 -m pip -V
```

Both commands will print the versions of `python3` and `pip` respectively.

Once both of those are installed, create a new folder for your project, something like `iot_course`, and inside download [this file](https://raw.githubusercontent.com/ssnover/iot-arduino-course/main/complete/mqtt-publisher/requirements.txt). Save it as `requirements.txt`. Then you can run `python3 -m pip install -r requirements.txt` from that directory to install the Python libraries we'll be using in this course.

## Python Development Environment

Feel free to develop and execute the python code in whatever way is most comfortable for you. However, if you've never written python before, I recommend a free program called `VSCode` for editing and running. Downloads for all platforms [are available here](https://code.visualstudio.com/download).

Once you've finished installing, navigate to the Extensions tab in the left column sidebar (icon shows three blocks together and an additional block by itself). There is an extension by Microsoft for Python which gives convenient ways of running Python scripts.
