#include "status.h"

#include "hwprofile.h"

void status_init(void)
{
  //Set pin directions
  STATUS_DIR_REG |= kStatusPinMask;
}

void status_set(uint8_t status_mode)
{
  STATUS_OUTPUT_REG |= (status_mode & kStatusPinMask);
}

void status_clear(uint8_t status_mode)
{
  STATUS_OUTPUT_REG &= ~(status_mode & kStatusPinMask);
}

void status_toggle(uint8_t status_mode)
{
  STATUS_OUTPUT_REG ^= (status_mode & kStatusPinMask);
}