#include <M5Stack.h>
#include <Wire.h>
#include <i2c.h>

bool i2cServoInit()
{
    bool found = false;
    // Initialize I2C and scan the bus
    Wire.setClock(400000); // Set I2C to 400 kHz (instead of 100 kHz)
    Wire.begin();
    Serial.println("Scanning (0x3A to be expected)...");
    for (byte i = 1; i < 127; i++)
    {
        Wire.beginTransmission(i);
        if (Wire.endTransmission() == 0)
        {
            Serial.print("Found I2C device at 0x");
            Serial.println(i, HEX);
            delay(10);
            if (i == 0x3A)
                found = true;
        }
    }
    return found;
}