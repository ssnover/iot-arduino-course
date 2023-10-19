#include "Arduino.h"
#include "Wire.h"
#include "AHT20.h"
#include <stdint.h>

const uint8_t AHT20_I2C_ADDR(0x38);

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("info: booting");
    Wire.begin();

    AHT20 aht20(AHT20_I2C_ADDR);

    bool initialized = aht20.begin();
    if (!initialized) {
        Serial.println("error: could not initialize aht20");
    } else {
        Serial.println("info: initialized sensor");
    }

    while (true) {
        while (!aht20.available()) {
          delay(10);
        }
        float temperature = aht20.getTemperature();
        Serial.print("data: ");
        Serial.println(temperature);
        delay(5000);
    }
}

void loop() {}
