#include "pwm.h"

#include "display.h" //Provides millis()
#include "hwprofile.h"
#include "status.h"

static uint16_t gPwmPeriod = 0;
static uint16_t gPwmLevel = 0;
static uint32_t gPwmPeriodStart = 0;

void pwm_init()
{
  //Set pin direction
  PWM_DIR_REG |= kPwmPinMask;
}

void pwm_set_period(uint16_t period)
{
  gPwmPeriod = period;
  gPwmLevel = 0;
  gPwmPeriodStart = millis();
}

void pwm_update(void)
{
  uint32_t timestamp = millis();
  if (timestamp > gPwmPeriodStart + gPwmPeriod)
    gPwmPeriodStart = timestamp;
  if (timestamp < gPwmPeriodStart + gPwmLevel) {
    //PWM Active
    PWM_OUTPUT_REG |= kPwmPinMask;
    status_set(kStatusHeat);
  } else {
    //PWM Inactive
    PWM_OUTPUT_REG &= ~kPwmPinMask;
    status_clear(kStatusHeat);
  }
}

void pwm_set_level(uint16_t level)
{
  gPwmLevel = level;
}

uint16_t pwm_period()
{
  return gPwmPeriod;
}