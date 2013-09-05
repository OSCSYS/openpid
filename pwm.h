#ifndef BOILPOWER_PWM_H_
#define BOILPOWER_PWM_H_

#include <stdint.h>

//Initialize PWM logic
void pwm_init(void);

//Configure PWM period
void pwm_set_period(uint16_t period);

//Update PWM logic
void pwm_update(void);

//Set the PWM on time in ms
void pwm_set_level(uint16_t level);

//Get the PWM Period
uint16_t pwm_period(void);

#endif
