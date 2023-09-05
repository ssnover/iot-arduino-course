# Reading the Serial Port in Python

In this chapter we'll be switching gears to write a host program in Python. Now that we've got serial data being emitted from the Arduino, we'll actually be able to read those bytes and manipulate them on the host side.

Create a new directory for the project, like `data_reader`, and add a `main.py` file to it. Download or copy [this file](https://raw.githubusercontent.com/ssnover/iot-arduino-course/main/complete/mqtt-publisher/requirements.txt) showing the required packages we'll need for this program. You can then install them into your hosted python environment with `python3 -m pip install -r requirements.txt`.

Inside `main.py`, we'll start off with a barebones python script:

```py
import sys

def main():
    """Entrypoint"""
    tty_path = "/dev/ttyACM0" # Change this to the port from the Arduino IDE
    if len(sys.argv) > 1:
        tty_path = sys.argv[1]
    
    print(f"Connecting to serial port at {tty_path}")


main()
```

At this point, all we're doing is defining a program which prints that it is connecting to a serial device (`tty_path`). I recommend populating a default for convenience; this should be based on whatever shows up as the Port in the Arduino IDE when the board is plugged in. We'll also parse the command line arguments so that that default can be overridden. The actual port can change depending on what USB port is used and what other devices are plugged in at any given time so this will prevent having to look it up all the time.

As a quick aside, the syntax in the print statement is called an f-string (short for "format string"), which is a little bit newer in python and as a result isn't as common to see in tutorials. It can make print statements a little bit more readable in some cases. This is how it looks with a normal string:

```py
print("Connecting to serial port at {0}".format(tty_path))
```

You can quickly try it for yourself with `python3 main.py /dev/ttyACM1` and verify that the override is working.

Now for the actual serial communication!

```py
import serial # New import for the pyserial library
import sys

def main():
    # --snip--
    print(f"Connecting to serial port at {tty_path}")

    serial_port = serial.Serial(tty_path, 115200, timeout=1)
    serial_port.reset_input_buffer()

    while True:
        next_line = serial_port.readline()
        if next_line:
            print(next_line)

# --snip--
```

Here we're using the pyserial library to get a connection to a serial port specified by its port name. We also specify the baud rate (`115200` baud) and a timeout. The timeout is specified in seconds and is important so that the program doesn't get stuck. This can happen because below we call `serial_port.readline()` which will not return until a line of data is ready, *unless* a timeout is specified in which case it will block for no longer than the timeout.

Since the attempt to read a line from the serial port can timeout, we also have to handle the case where the function timed out and we didn't get any data. So we only print the data if we get some data.

Make sure you've closed the Arduino IDE, then give this program a try! You should see a very similar output to the serial monitor, you've basically reimplemented it in python! You can close the program with `Ctrl+C`.

```
Connecting to serial port at /dev/ttyACM0
b'\r\n'
b'info: booting\r\n'
b'LED state: OFF\r\n'
b'LED state: ON\r\n'
b'LED state: OFF\r\n'
b'LED state: ON\r\n'
b'LED state: OFF\r\n'
b'LED state: ON\r\n'
```

Obviously, it's got a bit more decoration than the Serial Monitor display. This is how python prints encoded byte data, essentially assuming that it is ASCII. In order to turn it into a regular string, we'll need to decode it and probably remove the newline characters (`\r\n`). To do that, change `print(next_line)` to `print(next_line.decode().rstrip())`.

Before moving on, let's just quickly add some nicer handling for when `Ctrl+C` is pressed.

```py
import serial
import signal # New import
import sys

# Adding a global variable to track whether the program should be running
running = True

def exit_handler(sig, frame):
    """
    This function will be called when we press Ctrl+C.
    """
    global running
    # Setting the variable to false signals that the program should exit
    running = False


def main():
    # --snip--
    if len(sys.argv) > 1:
        tty_path = sys.argv[1]
    signal.signal(signal.SIGINT, exit_handler)

    # --snip--
    while running:
        next_line = serial_port.readline()
        if next_line:
            print(next_line.decode().rstrip())

    print("Exiting...")

# --snip--
```

In the above source, we're adding a function which handles a request to exit the program, just using a boolean variable to decide if the program should keep running or not. In our `main` function we configure the python environment to call our function `exit_handler` when `Ctrl+C` is pressed. The term for this is `SIGINT` so we're mapping from that signal.


In the next chapter, we'll go back to programming the Arduino IDE and actually tackle grabbing real sensor data! Just a quick recap of the full program, which is still quite small at this point:

```py
import serial
import signal
import sys

running = True

def exit_handler(sig, frame):
    """
    This function will be called when we press Ctrl+C.
    """
    global running
    # Setting the variable to false signals that the program should exit
    running = False


def main():
    """Entrypoint"""
    tty_path = "/dev/ttyACM0" # Change this to the port from the Arduino IDE
    if len(sys.argv) > 1:
        tty_path = sys.argv[1]
    print(f"Connecting to serial port at {tty_path}")

    serial_port = serial.Serial(tty_path, 115200, timeout=10)
    serial_port.reset_input_buffer()

    while running:
        next_line = serial_port.readline()
        if next_line:
            print(next_line.decode().rstrip())

    print("Exiting...")


main()
```