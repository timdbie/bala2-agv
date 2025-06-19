#include <pi.h>
#include <servo.h>
#include <M5Stack.h>
int prev_pos_left = 0;
int prev_pos_right = 0;
float integral_err_left = 0;
float integral_err_right = 0;

void read_motor_velocity(int32_t* left, int32_t* right)
{
  int curr_pos_left, curr_pos_right;
  read_encoders(&curr_pos_left, &curr_pos_right);

  *left = (curr_pos_left - prev_pos_left) / deltaTime;
  *right = (curr_pos_right - prev_pos_right) / deltaTime;

  prev_pos_left = curr_pos_left;
  prev_pos_right = curr_pos_right;
}

void pi_control(int32_t target_velocity_left, int32_t target_velocity_right, int32_t* velocity_left, int32_t* velocity_right)
{
  read_motor_velocity(velocity_left, velocity_right);

  //error calc
  float err_left = target_velocity_left - *velocity_left;
  float err_right = target_velocity_right - *velocity_right;

  integral_err_left += err_left * deltaTime;
  integral_err_right += err_right * deltaTime;

  //output
  float output_left = err_left * Kp + integral_err_left * Ki;
  float output_right = err_right * Kp + integral_err_right * Ki;

  int pwm_left = constrain((int)output_left, -1023, 1023);
  int pwm_right = constrain((int)output_right, -1023, 1023);
  write_motor_cmd(pwm_left, pwm_right);
}