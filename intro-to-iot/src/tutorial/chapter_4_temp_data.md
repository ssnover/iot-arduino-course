# Collecting Temperature Data

Now that we've verified sending serial data from the Arduino to our python script, let's read some real data! For this tutorial, we're using an AHT20 temperature sensor which we can read data from via the I2C protocol (pronounced "I squared C" or "I two C").

For legal reasons that I won't pretend to understand comprehensively, there are a number of "I2C-compatible" protocols and devices which implement the same protocol but are just called different things. For the Atmega328 (the microcontroller running on the Arduino UNO), it's called two-wire interface (TWI), but pretty much everywhere you'll see it referred to as I2C.

There's a [library from Adafruit](https://github.com/adafruit/Adafruit_AHTX0) for the sensor used here. However, this is fairly simple sensor so we'll roll our own as an exercise in reading a datasheet for a device like this. An online copy of the datasheet is [here](https://files.seeedstudio.com/wiki/Grove-AHT20_I2C_Industrial_Grade_Temperature_and_Humidity_Sensor/AHT20-datasheet-2020-4-16.pdf). You can usually find a link to the datasheet on the product page of the store where you bought the sensor.

In the datasheet, the sections we're interested in are Section 5 which describes the commands used to communicate with the sensor and Section 6 which describes how to take the raw binary number reported by the sensor and turn it into a temperature in degrees Celsius. I'll be referring to these sections throughout the chapter.

The first piece of information we need is the I2C address. I2C is a bus protocol where multiple devices can be attached to two signals: the clock line and data line. The address is used so that a device knows that the data being sent on the bus is intended for it. Section 5.3 mentions that the I2C address is `0x38`.

With the address, this device also accepts certain commands which are each represented with a byte. The same section includes a table of the commands. The relevant ones for us are "Initialization" and "Trigger measurement", we'll ignore the last one. Reading the next section, 5.4, actually reveals an additional command which will be important, getting the status register. A lot of these datasheets are pretty poorly written and this is a pretty good example. That's another reason to find a library if one exists, to use it as a reference when something in the datasheet doesn't make sense.

The initialization process called for is to wait for 40 ms for the device to power up, then check the status byte. If bit 3 of the status byte is set (or it's a `1`), then it is calibrated. If not, we need to calibrate it with the Initialization command.

Now let's implement what we've read out of the datasheet:

```cpp
#include "Wire.h"
#include <stdint.h>

const uint8_t AHT20_I2C_ADDR(0x38);
const uint8_t GET_STATUS(0x71);
const uint8_t CALIBRATE_CMD(0xbe);
const unsigned int SENSOR_INIT_DELAY(40);
const unsigned int SENSOR_CALIBRATION_DELAY(10);
```

First we'll pull in the Arduino's I2C library with in `Wire.h` and we'll pull in `stdint.h` which is a standard C library that defines a bunch of fixed-size types. This allows specifying that a variable is a `uint8_t` instead of an `unsigned int`. The former will always be a single byte (8 bits). The latter can change it's size based on the hardware that's being compiled to. Because every byte counts when you're working with hardware and if you send an extra byte the command you send to a sensor won't be understood at all! Define a bunch of constants for the I2C address and the two commands we need to initialize the sensor.

Next we'll write a function to get the status since that's the first thing we need to do:

```cpp
uint8_t get_status(uint8_t address) {
    Wire.requestFrom(address, 1);
    while (!Wire.available()) {
        delay(1);
    }
    uint8_t status = Wire.read();
    return status;
}
```

Since we're getting the status from the sensor, we're doing a read command with `Wire.requestFrom()`. We pass the address of the device we're talking to and the number of bytes we want back from the device. Then we have to wait for the response. You can call `Wire.available()` to check if there's any data that's been received (it actually returns the number of bytes received), then read it with `Wire.read()`.

Next we'll use this to implement an initialization function:

```cpp
void init_aht20_sensor(uint8_t address) {
    delay(SENSOR_INIT_DELAY);
    
    uint8_t status = get_status(address);
    bool is_calibrated = status & (1u << 3);

    if (!is_calibrated) {
        const uint8_t cmd_buffer[3] = {CALIBRATE_CMD, 0x08, 0x00};
        Serial.println("debug: calibrating sensor");

        Wire.beginTransmission(address);
        Wire.write(&cmd_buffer[0], 3);
        Wire.endTransmission();
        
        delay(SENSOR_CALIBRATION_DELAY)
    }

    Serial.println("debug: sensor ready");
}
```

So we first call the function we just implemented `get_status()`, however that just returns a byte. In order to get the value of a specific bit, we'll need to do some bit-level logic and masking. These aren't commonly used in desktop programs so I'll go into a little more detail here. First the rightmost expression, `(1u << 3)`. This takes a value of `0b0001` in binary and applies a shift left of 3 bit positions. If we shift the bits to the left 3 times, that `0b0001` becomes `0b1000`. Zeroes fill in the spots where a new value is required (below bit 0). 

Next we do a bit-wise logical AND between this value (the bitmask) and the status byte. That looks like `status & 0b0000_1000`. Anything ANDed with 0 is 0 and anything ANDed with 1 is itself. So the result of that entire expression can be expressed as `0b0000_x000` where x is 1 or 0 depending on if the sensor is calibrated or not. If it's a 1, casting this to a `bool` will give it a value of `true`, otherwise it will be `false`.

In the case where it's not calibrated, we need to send a calibration command. The datasheet describes it as a 3 bytes: the calibration command byte followed by `0x08 0x00`. Writing data to a sensor is a little more verbose. Start a write with `Wire.beginTransmission()`, write the data buffer of 3 bytes with `Wire.write()`, then end it with `Wire.endTransmission()`.

That should be sufficient to initialize the sensor, let's add it to our program:

```cpp
// --snip--

void init_aht20_sensor(uint8_t address);
uint8_t get_status(uint8_t address);

void setup() {
    // --snip--
    Serial.println("info: booting");

    Wire.begin();
    init_aht20_sensor(AHT20_I2C_ADDR);
    Serial.println("info: initialized sensor");

    while (true) {
        // --snip--
    }
}

void init_aht20_sensor() {
    // --snip--
}

uint8_t get_status() {
    // --snip--
}
```

To wrap up the integration of the initialization routine, we just add a call to `Wire.begin()` to initialize the Arduino's I2C hardware similar to serial's `Serial.begin()`, then call our function.

I recommend building and running the code really quick to verify you've got it working (and/or that the wiring between the Arduino and the sensor is correct). In your python program, you should see:

```
info: booting
debug: calibrating sensor // this may be missing as it won't always be required
debug: sensor ready
info: initialized sensor
LED state: OFF
LED state: ON
LED state: OFF 
```

With the initialization out of the way, let's read some data. The figure in the datasheet shows the contents and length of the data to be read back. This sensor measures both temperature and humidity so the measurement will contain both values. Out of the bytes returned the temperature data can be found in the 4th, 5th, and 6th byte and it has a size of 20 bits. The next size type up is a `uint32_t` so we'll plan to store it in that.

```cpp
float request_temperature_data(uint8_t address) {
    // send a command
    const uint8_t cmd_buffer[3] = {TRIGGER_CMD, 0x33, 0x00};
    Wire.beginTransmission(address);
    Wire.write(&cmd_buffer[0], 3);
    Wire.endTransmission();

    bool data_available = false;
    while (!data_available) {
        delay(10);
        uint8_t status = get_status();
        if (!(status & (1u << 7))) {
            data_available = true;
        }
    }

    // --snip next section--
}
```

First we need to send the command to trigger a measurement which is very similar to the calibrate command. The measurement takes some time to complete and we can determine when it's done by reading the status byte again. This time it's bit 7 instead of bit 3 so we adjust the masking appropriately, but there's one extra complication: 1 in this position indicates the sensor is busy and 0 means it's done, so we need to invert the logic. Be careful with the placement of your parentheses here, these can be very difficult problems to debug. I once had a bug that took several days to find because I had `~byte` instead of `!byte` (bitwise NOT instead of boolean NOT). We'll just loop until data is available.

```cpp
float request_temperature_data(uint8_t address) {
    // --snip previous section--

    const uint8_t READING_BYTES = 7;
    uint8_t reading[READING_BYTES] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Wire.requestFrom(address, READING_BYTES);
    for (auto bytes_read = 0u; bytes_read < READING_BYTES; ++bytes_read) {
        if (Wire.available()) {
            reading[bytes_read] = Wire.read();
        } else {
            delay(1);
        }
    }

    // --snip next section--
}
```

Then we can proceed to read the raw data from the sensor. The datasheet mentions that the payload size if 7 bytes and we've already determined which of those bytes we're interested in, but we'll just read them all. We loop through and fill a buffer.

```cpp
float request_temperature_data(uint8_t address) {
    // --snip previous section--

    uint32_t raw = ((uint32_t)(reading[3] & 0b00001111) << 16);
    raw |= ((uint32_t)reading[4] << 8);
    raw |= reading[5];
    float temperature = calculate_temperature_from_raw(raw);
    return temperature;
}
```

The last part is the most complicated. The first part to notice is the the 4th byte (`reading[3]`) actually contains temperature and humidity data. We only want the lowest 4 bits on account of this. But are these the lowest 4 bits of the temperature data or the highest 4 bits? The datasheet is remarkably unhelpful here, but it appears to follow convention: big-endian for protocol data. That means the most significant bytes are sent first. If you're ever unsure, try one way and see how the value changes at rest. If it's backwards, you'll see rapid swings in the data as the least significant bits will change frequently with noise on the sensor and they're in the wrong position.

Since the 4 bits are the most significant, after masking them we'll shift them 16 places to leave room for the remaining 16 bits. Before we can shift though, we need to make sure the data being shifted fits into a data type that can fit all of the bits (remember that `reading` is an array of bytes). We can do so by casting the data to a 32-bit wide type with `(uint32_t)(data)`. Then we continue building up our raw measurement value with the two other bytes, which do not require masking. The syntax `raw |= data;` is shorthand for `raw = raw | data` and since we left space there, this will make that region of the data equal to the byte we're reading from.

Our raw data isn't very useful as it's quite hard to read and understand, so we'll take one last step and calculate the temperature in units of degrees Celsius. We'll be referring to Section 6.2 of the datasheet for the formula.

```cpp
float calculate_temperature_from_raw(uint32_t raw_data) {
    // See Section 6.2
    auto ratio = static_cast<float>(raw_data) / (1ul << 20);
    ratio *= 200.;
    ratio -= 50.;
    return ratio;
}
```

The first step of the calculation involves dividing by the full possible resolution, effectively giving us a value between 0.0 and 1.0. We'll need to cast the data to a floating point value which can represent decimals, otherwise the result will always be 0. Note that I'm using shifting for the denominator again. In binary, shifting a number to the left by 1 is equivalent to multiplying it by 2, just as shifting a number to the left in decimal one place is equivalent to multiplying by 10.

Then we just multiply by 200 and subtract 50. This means the full range of temperature values that it's possible for the sensor to represent is -50 C to 150 C.

Let's add the measurement code to our program's mainloop!

```cpp
void setup() {
    // --snip--

    while (true) {
        digitalWrite(LED_PIN, led_state);
        led_state = !led_state;

        float temperature = request_temperature_data();
        Serial.print("data: ");
        Serial.println(temperature);
        delay(5000);
    }
}
```

I've removed the logging of the LED state as it's not very useful for our python program. We'll let the LED continue to blink though since it can be useful to know if the program is running at a glance.

That's it! If you run your python program or check the Serial Monitor, you should see temperature values being reported in the terminal!

```
TODO
```

In the next chapter we'll modify our python program to parse that data from a string in the format `"data: 19.4"` into a decimal number that can be plotted or have other analysis performed on it.