#include "Arduino.h"
#include "Wire.h"
#include <stdint.h>

const uint8_t AHT20_I2C_ADDR(0x38);
const uint8_t CALIBRATE_CMD(0xe1);
const uint8_t TRIGGER_CMD(0xac);

bool init_aht20_sensor();
uint8_t get_status();
float request_temperature_data();
float calculate_temperature_from_raw(uint32_t raw_data);

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("info: booting");
    Wire.begin();
    bool initialized = init_aht20_sensor();
    if (!initialized) {
        Serial.println("error: could not initialize aht20");
    } else {
        Serial.println("info: initialized sensor");
    }

    while (true) {
        float temperature = request_temperature_data();
        Serial.print("data: ");
        Serial.println(temperature);
        delay(5000);
    }
}

bool init_aht20_sensor() {
    const uint8_t cmd_buffer[3] = {CALIBRATE_CMD, 0x08, 0x00};
    Serial.println("debug: writing calibrate cmd");
    Wire.beginTransmission(AHT20_I2C_ADDR);
    Wire.write(&cmd_buffer[0], 3);
    Wire.endTransmission();

    bool is_ready = false;
    while (!is_ready) {
        uint8_t status = get_status();
        if (status & 0x08) {
            is_ready = true;
            Serial.println("debug: sensor ready");
        }
    }

    return true;
}

uint8_t get_status() {
    Wire.requestFrom(AHT20_I2C_ADDR, 1);
    while (!Wire.available()) {
        delay(1);
    }
    uint8_t status = Wire.read();
    return status;
}

float request_temperature_data() {
    // send a command
    const uint8_t cmd_buffer[3] = {TRIGGER_CMD, 0x33, 0x00};
    Wire.beginTransmission(AHT20_I2C_ADDR);
    Wire.write(&cmd_buffer[0], 3);
    Wire.endTransmission();

    bool data_available = false;
    while (!data_available) {
        delay(10);
        uint8_t status = get_status();
        if (status & 0x80) {
            data_available = true;
        }
    }

    const uint8_t READING_BYTES = 6;
    uint8_t reading[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    Wire.requestFrom(AHT20_I2C_ADDR, READING_BYTES);
    for (auto bytes_read = 0u; bytes_read < READING_BYTES; ++bytes_read) {
        if (Wire.available()) {
            reading[bytes_read] = Wire.read();
        } else {
            delay(1);
        }
    }

    uint32_t raw = ((uint32_t)(reading[3] & 0x0F) << 16);
    raw |= ((uint32_t)reading[4] << 8);
    raw |= reading[5];
    float temperature = calculate_temperature_from_raw(raw);
    return temperature;
}

float calculate_temperature_from_raw(uint32_t raw_data) {
    auto ratio = static_cast<float>(raw_data) / (1ul << 20);
    ratio *= 200.;
    ratio -= 50.;
    return ratio;
}

void loop() {}
