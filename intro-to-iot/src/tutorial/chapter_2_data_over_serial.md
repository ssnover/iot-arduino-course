# Reporting Data Over Serial

Next, we're going to cover getting data off the Arduino and onto devices that talk to it. This is what actually makes an embedded system an Internet of Things (IoT) device. The device needs to be connected, directly or indirectly, to the network in some way such that other applications can make use of the data it produces or send commands to the device.

The technology we'll be making use of here is called serial (or UART if you want to research the details). I'll not be explaining in-depth how serial works here. Instead I'll be focusing exclusively on how we are to use it and what it provides us.

Serial is means of sending data between two devices as a series (keyword here) of bytes. Back in the day, many desktop computers had hardware on board for talking to serial devices directly (using a DB-9 connector), but on modern hardware we usually connect to serial devices over USB. Fortunately, the Arduino has the circuitry on board in order to send serial data over USB.

For this chapter, we'll be looking exclusively at how to send data from the Arduino using serial. We'll use the Arduino IDE's Serial Monitor tool to see the information we send. See the next chapter for writing a program to actually receive that data and do something with it.

From last chapter, we're starting here:

```cpp
void setup() {
    // Initialization
    const int LED_PIN = 13;
    pinMode(LED_PIN, OUTPUT);
    bool led_state = false;

    // Main loop
    while (true) {
        digitalWrite(LED_PIN, led_state);
        led_state = !led_state;
        delay(5000);
    }
}
```

To the initialization section, we'll be adding some code for setting up the Arduino's serial hardware:

```cpp
// Initialization
// --snip--
Serial.begin(115200);
Serial.println();
Serial.println("info: booting");

while (true) {
// --snip--
```

I've omitted most of the code from before, but make sure to add this new code above the while loop.

Here, we tell the microcontroller to start up the serial port hardware with `Serial.begin(115200);`. Specifically, we're telling it to start up with a baud rate of 115200 bits per second. This is the frequency of serial communication and you don't need to give it much thought other than to remember that number since it's important that both devices talking serial are operating with the same frequency.

Next, we use the serial port to send some simple text (or strings) to any device listening on the other side. Go ahead and compile (verify) and flash (upload) that code. Then open the Serial Monitor (`Tools` -> `Serial Monitor` in the Arduino IDE menu).

In the serial monitor, select `115200 baud` in the rightmost dropdown menu. If you forget, and have something like `9600 baud`, you'll probably see some question mark-like characters. You'll also need to hit the button marked "Reset" on your Arduino to see the boot message we added to initialization. This is because connecting the Serial Monitor doesn't actually reboot the program and our program has been running since the flashing completed!

Next up, let's print the status of the LED in our main loop!

```cpp
// --snip--
while (true) {
    digitalWrite(LED_PIN, led_state);

    Serial.print("LED state: ");
    if (led_state) {
        Serial.println("ON");
    } else {
        Serial.println("OFF");
    }

    led_state = !led_state;
    delay(5000);
}
```

Notice that for the first statement I used `Serial.print` instead of `Serial.println`! The difference between these two is that `println` does everything `print` does, except it also sends a special character sequence which means that the next data sent should be printed on the next line. If you're familiar with strings in C, you'll know that this is either `\n` or `\r\n`. The details of that aren't incredibly important just yet, but making use of `print` and `println` effectively can make reading data in the Serial Monitor easier.

When you build and run the code, you should see something like this in your Serial Monitor:

```
info: booting
LED state: OFF
LED state: ON
LED state: OFF
LED state: ON
LED state: OFF
LED state: ON
```

Congratulations! You've taken the first steps towards reporting data from hardware to your computer! In the next chapter we'll switch gears and focus on using Python to read that data. The full Arduino code up to this point is shown below:

```cpp
void setup() {
    // Initialization
    const int LED_PIN = 13;
    pinMode(LED_PIN, OUTPUT);
    bool led_state = false;

    Serial.begin(115200);
    Serial.println();
    Serial.println("info: booting");

    // Main loop
    while (true) {
        digitalWrite(LED_PIN, led_state);
        led_state = !led_state;

        Serial.print("LED state: ");
        if (led_state) {
            Serial.println("ON");
        } else {
            Serial.println("OFF");
        }

        delay(5000);
    }
}

void loop() {}
```