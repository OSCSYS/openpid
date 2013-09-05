#ifndef BOILPOWER_CALCS_H_
#define BOILPOWER_CALCS_H_

#include <stdint.h>

//Calculates minimum required sensitivity to constrain range < 255
uint8_t calcs_minimum_sensitivity(uint8_t sensitivity, uint8_t period, uint8_t frequency);

//Calculates number of encoder increments
uint8_t calcs_range(uint8_t period, uint8_t frequency, uint8_t sensitivity);

//Calculates PWM value as ms
uint16_t calcs_pwm_time(uint16_t periodMs, uint8_t value, uint8_t range);

//Calculates PWM value as tenths of percent (ie 31/40 = 775 or 77.5%)
uint16_t calcs_pwm_percent(uint8_t value, uint8_t range);

#endif