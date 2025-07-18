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
#include <delay.h>
#include <servo.h>
#include <pi.h>
#include <i2c.h>

#define MAX_VAL 1023
// #define MAX_INTEGRAL 1000

#define LEFT_IR 26
#define RIGHT_IR 36

float target_velocity_left = 0;
float target_velocity_right = 0;
float Kp = 0.05;
float Ki = 1;
float deltaTime = 0.05;

void setup() {
  bool found = false;

  M5.begin();
  M5.Lcd.println("Hello from M5Stack!");

  Serial.begin(115200);
  Serial.println("Hello from M5Stack (serial port)!");

  pinMode(LEFT_IR, INPUT_PULLUP);
  pinMode(RIGHT_IR, INPUT_PULLUP);
  
  if(i2cServoInit())
  {
    M5.Lcd.println("Motor controller found on I2C bus!");
  }
  else
  {
    M5.Lcd.println("*** ERROR *** Unable to locate the motor controller on I2C bus at 0x3A.");
    M5.Lcd.println("*** ERROR *** Check your hardware first.");
    while(1);
  }
}

void loop() {
  uint32_t elapsed;
  if (intervalReady(&elapsed)) {
    int left_sensor = digitalRead(LEFT_IR);
    int right_sensor = digitalRead(RIGHT_IR);

    if (left_sensor == HIGH && right_sensor == LOW) {
      // line detected on left sensor only
      target_velocity_left = 0;  
      target_velocity_right = 200; 
    }
    else if (right_sensor == HIGH && left_sensor == LOW) {
      // line detected on right sensor only
      target_velocity_left = 200; 
      target_velocity_right = 0;  
    }
    else if (left_sensor == LOW && right_sensor == LOW) {
      // no line detected
      target_velocity_left = 0;
      target_velocity_right = 0;
    }
    else if (left_sensor == HIGH && right_sensor == HIGH) {
      // line detected
      target_velocity_left = 200;
      target_velocity_right = 200;
    }
    else {
      // fallback in case of unexpected sensor readings
      target_velocity_left = 0;
      target_velocity_right = 0;
    }

    int32_t velocity_left;
    int32_t velocity_right;
    pi_control(target_velocity_left, target_velocity_right, &velocity_left, &velocity_right);
  }
}
