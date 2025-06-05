#include <M5Stack.h>
#include <Wire.h>

int32_t right_prev = 0;
uint32_t time_prev = 0;
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
  Serial.println("Time(ms),Input,Velocity");
}

void loop() {
  static uint32_t startTime = millis();
  int32_t left, right;
  read_encoders(&left, &right);
 
  uint32_t now = millis();
  uint32_t elapsed = now - startTime;
  uint32_t dt = now - time_prev;
  int16_t stepInput = 500;
  float velocity = (right - right_prev) / (dt / 1000.0);
  if (elapsed > 5000) {
    write_motor_cmd(0, 0);
    return;
  }

  write_motor_cmd(0, stepInput);
  Serial.print(elapsed);
  Serial.print(", ");
  Serial.print(stepInput);
  Serial.print(", ");
  Serial.println(velocity);
  right_prev = right;
  time_prev = now;
  delay(50);
}