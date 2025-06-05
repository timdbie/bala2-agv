/*
Make sure you have the [Arduino IDE](https://www.arduino.cc/en/software/) installed.
To program your bala2, prepare the following:

1. **Add the ESP32 board URL**  
   Go to `File > Preferences`, and in the **"Additional Board Manager URLs"** field, add:  
https://espressif.github.io/arduino-esp32/package_esp32_index.json

2. **Install ESP32 board definitions**  
Go to `Tools > Board > Boards Manager` (or press `Ctrl+Shift+B`), search for **ESP32 by Espressif Systems**, select **version 2.0.7**, and click **Install**.

3. **Select the correct board**  
Navigate to `Tools > Board > esp32`, then select **M5Stack-Core-ESP32**  
*(Note: boards may not be listed in alphabetical order.)*

4. **Install required libraries**  
Go to `Tools > Manage Libraries` (or press `Ctrl+Shift+I`), search for **M5Stack**, and install the library.

5. **Verification**
Verify your installation together with the Bala2 robot by programming the running "hello world" application:

*/
#include <M5Stack.h>
#include <Wire.h>

void read_encoders(int32_t* wheel_left_encoder, int32_t* wheel_right_encoder)
{
  uint8_t data_in[8];
  M5.I2C.readBytes(0x3A, 0x10, 8, data_in);

  *wheel_left_encoder = (data_in[0] << 24) | (data_in[1] << 16) | (data_in[2] << 8) | data_in[3];
  *wheel_right_encoder = (data_in[4] << 24) | (data_in[5] << 16) | (data_in[6] << 8) | data_in[7]; 
}

void write_motor_cmd(int16_t wheel_left, int16_t wheel_right) // Sets the motor setpoint (range: -1023 ~ 1023)
{
  uint8_t data_out[4];

  // Invert the command (positive command should result in an increment in encoder position)
  wheel_left *= -1;
  wheel_right *= -1;

  data_out[0] = (int8_t)(wheel_left >> 8);
  data_out[1] = (int8_t)(wheel_left >> 0);
  data_out[2] = (int8_t)(wheel_right >> 8);
  data_out[3] = (int8_t)(wheel_right >> 0);

  M5.I2C.writeBytes(0x3A, 0x00, data_out, 4);
}

void setup() {
  bool found = false;

  M5.begin();
  M5.Lcd.println("Hello from M5Stack!");

  Serial.begin(115200);
  Serial.println("Hello from M5Stack (serial port)!");
  
  // Initialize I2C and scan the bus
  Wire.setClock(400000);        // Set I2C to 400 kHz (instead of 100 kHz)
  Wire.begin();
  Serial.println("Scanning (0x3A to be expected)...");
  for (byte i = 1; i < 127; i++) {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found I2C device at 0x");
      Serial.println(i, HEX);
      delay(10);
      if(i == 0x3A) 
        found = true;
    }
  }

  if(found)
  {
    M5.Lcd.println("Motor controller found on I2C bus!");
    M5.Lcd.println("Rotate the left wheel now to control the right SYSTEMIDENTIFICATION");
  }
  else
  {
    M5.Lcd.println("*** ERROR *** Unable to locate the motor controller on I2C bus at 0x3A.");
    M5.Lcd.println("*** ERROR *** Check your hardware first.");
    while(1);
  }
}

void loop() {
  int32_t left, right;
 
  read_encoders(&left, &right);
  Serial.print(left);
  Serial.print(", ");
  Serial.println(right);

  write_motor_cmd(0, left);
  delay(1000);
}
