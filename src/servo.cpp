#include <M5Stack.h>
#include <servo.h>
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