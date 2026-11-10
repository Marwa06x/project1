#include <Arduino.h>
#include <Wire.h>

void setup() {
    Serial.begin(115200);
    Wire.begin(8, 9);  // SDA, SCL
    Wire.setClock(100000);
}

void loop() {
    bool found = false;

    for (uint8_t address = 0x76; address <= 0x77; address++) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
            Serial.printf("Sensor responded at 0x%02X\n",
                          static_cast<unsigned int>(address));
            found = true;
        }
    }

    if (!found) {
        Serial.println("No sensor response at 0x76 or 0x77");
    }

    delay(2000);
}