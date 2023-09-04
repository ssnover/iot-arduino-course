# Blinking an LED

We'll get started on the Arduino side of things first since that will eventually be the source of the data. This is always a useful first step no matter how experienced you are with embedded systems as it verifies that your compiler and flashing toolchain is functional and that the basics of your hardware setup are correct. Luckily the Arduino UNO has an LED onboard for expressly this purpose.

Open your new sketch in the Arduino IDE, call it `temperature_sensor` or something like that. This should give you a file with two functions defined: `setup` and `loop`. Rather than defining a `main` function yourself, Arduino typically tries to hide some of these details in their attempt to be more user-friendly. I usually leave the `loop` function empty and treat `setup` as my `main` function. This means you don't need to define every as a global variable (since there's otherwise no way to reference variables in both functions).

## Source Code

Let's start with this implementation to get an LED blinking, I'll explain what each line is doing down below.

```cpp
void setup() {
    const int LED_PIN = 13;
    pinMode(LED_PIN, OUTPUT);
    bool led_state = false;

    while (true) {
        digitalWrite(LED_PIN, led_state);
        led_state = !led_state;
        delay(5000);
    }
}
```

The code can be explained in two parts. First we have the initialization step, making a constant variable with a good name of `LED_PIN` and assigning it to `13` since the Arduino's onboard LED is pin `D13`. We have to configure that GPIO pin on the microcontroller as an output (instead of an input) so that we can set that GPIO pin to logical high or low states (5 volts or 0 volts respectively). This is done with the `pinMode` function, whose documentation can be found [here](https://www.arduino.cc/reference/en/language/functions/digital-io/pinmode/). Finally, we create a variable to represent the current state of the LED (`false` meaning off).

Next there's the looping code which will run forever as long as the board is powered (due to `while (true) {}`). In here, we use `digitalWrite` to set the LED's pin to our current LED state, then we flip the LED state with boolean logic. Finally, we add a delay since otherwise it would blink so fast that we'd never see it. The `delay` function takes an argument of milliseconds, so in this case there will be 5 seconds between changes to the LED.

## Flashing Code to the Arduino

Use the Verify (checkmark) to compile the C++ source code (make sure you've chosen your Board first) and wait for a successful compile. If you've missed a semicolon or have another type of syntax error, the Output window on the bottom will show red text explaining the error that prevented compiling. It will also show warnings here in yellow text which can provide clues if your code doesn't function how you expect.

Plug in the board's USB cable to your computer and select the board's port via `Tools` -> `Port`. On Windows, it will be something like `COM1`. On Linux, it will be something like `/dev/ttyACM0`. On Mac, it will be something like `<TODO>`. Once the port is chosen, you can flash the board with Upload button (has an `->` in its icon).

A successful Upload will have text like this in the Output window:
```red
avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.00s

avrdude: Device signature = 0x1e950f (probably m328p)
avrdude: reading input file "/tmp/arduino/sketches/E9A94DB52016B8DEAFA0AB134D40CC04/sketch_sep3a.ino.hex"
avrdude: writing flash (882 bytes):

Writing | ################################################## | 100% 0.14s

avrdude: 882 bytes of flash written

avrdude done.  Thank you.
```

You should now see a blinking LED on your Arduino. Verify the period of the blinking matches what you expect with a watch or by counting. You can also change the `5000` to some other numbers and re-upload to verify that it updated.