#include "ui.h"

#include <stdint.h>
#include <string.h>

#include "calcs.h"
#include "display.h"
#include "encoder.h"
#include "hwprofile.h"
#include "pwm.h"
#include "status.h"
#include "onewire.h"

enum UiState {
  kUiStateOff,
  kUiStateOn,
  kUiStateU1,
  kUiStateU2,
  kUiStateU3,
  kUiStateNumStates
};

void ui_state_enter(enum UiState state);
void ui_update_value(uint8_t value);
void ui_lock(void);
void ui_unlock(void);
uint8_t ui_setup_period(struct BoilPowerSettings *settings);
uint8_t ui_setup_sensitivity(struct BoilPowerSettings *settings);
uint8_t ui_setup_frequency(struct BoilPowerSettings *settings);
uint8_t ui_setup_user1(struct BoilPowerSettings *settings);
uint8_t ui_setup_user2(struct BoilPowerSettings *settings);
uint8_t ui_setup_user3(struct BoilPowerSettings *settings);
uint8_t ui_setup_hotlock(struct BoilPowerSettings *settings);
uint8_t ui_setup_reset(struct BoilPowerSettings *settings);
uint8_t ui_setup_save(struct BoilPowerSettings *settings);
uint16_t ui_get_value(uint16_t value, uint8_t minValue, uint8_t maxValue, uint8_t decimalPosition, uint16_t (*calc_function)(uint8_t, uint8_t));
uint8_t ui_get_yes_no(uint8_t value, char displayYes[], char displayNo[]);

struct menuItem {
  char title[4];
  uint8_t (*menuFunc)(struct BoilPowerSettings*);
};

static const struct menuItem kSettingsMenu[] = {
  {"Prd", ui_setup_period},
  {"SEN", ui_setup_sensitivity},
  {"Frq", ui_setup_frequency},
  {" U1", ui_setup_user1},
  {" U2", ui_setup_user2},
  {" U3", ui_setup_user3},
  {"Hot", ui_setup_hotlock},
  {"rSt", ui_setup_reset},
  {"SEt", ui_setup_save}
};

static struct BoilPowerSettings *gUiSettings;
static enum UiState gUiState = kUiStateOff;
static uint8_t gUiLocked = 1;

void ui_init(struct BoilPowerSettings *settings)
{
  gUiSettings = settings;
  encoder_set_limits(0, calcs_range(gUiSettings->data.period, gUiSettings->data.frequency, gUiSettings->data.sensitivity));
  encoder_set_value(0);
  ui_state_enter(kUiStateOff);
  ui_lock();
}

void ui_update()
{
  if (gUiLocked) {
    if (encoder_cancel()) 
      ui_unlock();
    encoder_ok(); //Dummy check to clear Enter
  } else {
    if (encoder_cancel())
      ui_lock();
    if (encoder_ok())
      ui_state_enter(gUiState + 1);
    if(encoder_changed())
      ui_update_value(encoder_value());
  }
}

void ui_state_enter(enum UiState state)
{
  gUiState = state;
  switch (gUiState) {
  case kUiStateOff:
    ui_update_value(0);
    encoder_set_value(0);
    break;
  case kUiStateOn:
    ui_update_value(encoder_maximum());
    encoder_set_value(encoder_maximum());
    break;
  case kUiStateU1:
  case kUiStateU2:
  case kUiStateU3:
    if (gUiSettings->data.userSetpoint[gUiState - kUiStateU1]) {
      ui_update_value(gUiSettings->data.userSetpoint[gUiState - kUiStateU1]);
      encoder_set_value(gUiSettings->data.userSetpoint[gUiState - kUiStateU1]);
    } else {
      ui_state_enter(gUiState + 1);
    }
    break;
  default:
    ui_state_enter(kUiStateOff);
    break;
  }
}

void ui_update_value(uint8_t value)
{
  if (gUiState >= kUiStateU1 && gUiState <= kUiStateU3)
    gUiSettings->data.userSetpoint[gUiState - kUiStateU1] = value;
  uint8_t maximum = encoder_maximum();
  if (!value)
    display_write_string("Off");
  else if (value == maximum)
    display_write_string(" On");
  else
    display_write_number(calcs_pwm_percent(value, maximum), 1);
  pwm_set_level(calcs_pwm_time(pwm_period(), value, maximum));
}

void ui_lock()
{
  if(!gUiSettings->data.hotLock)
    ui_state_enter(kUiStateOff);
  uint8_t value = encoder_value();
  //Lock encoder to current value
  encoder_set_limits(value, value);
  status_set(kStatusLock);
  gUiLocked = 1;
}

void ui_unlock()
{
  //Restore normal encoder range
  encoder_set_limits(0, calcs_range(gUiSettings->data.period, gUiSettings->data.frequency, gUiSettings->data.sensitivity));
  status_clear(kStatusLock);
  gUiLocked = 0;
}

void ui_setup(struct BoilPowerSettings *settings)
{
  enum {
    kMenuStateInit,
    kMenuStateUpdate,
    kMenuStateReady
  } menuState = kMenuStateInit;
  
  uint8_t menuPos = 0;
  
  while(1) {
    switch (menuState) {
      case kMenuStateInit:
        encoder_set_limits(0, sizeof(kSettingsMenu) / sizeof(kSettingsMenu[0]) - 1);
        encoder_set_value(menuPos);
        
      case kMenuStateUpdate:
        display_write_string(kSettingsMenu[menuPos].title);
        menuState = kMenuStateReady;
        
      case kMenuStateReady:
        if(encoder_changed()) {
          menuPos = encoder_value();
          menuState = kMenuStateUpdate;
        }
        if(encoder_ok()) {
          if(kSettingsMenu[menuPos].menuFunc(settings))
            return; //Exit Signal
          menuState = kMenuStateInit;
        }
        encoder_cancel(); //Dummy Check to Clear Cancel Status
    }
  }
}

uint8_t ui_setup_period(struct BoilPowerSettings *settings)
{
  settings->data.period = ui_get_value(settings->data.period, 1, 255, 1, 0);
  settings->data.sensitivity = calcs_minimum_sensitivity(settings->data.sensitivity, settings->data.period, settings->data.frequency);
  return 0;
}

uint8_t ui_setup_sensitivity(struct BoilPowerSettings *settings)
{
  settings->data.sensitivity = ui_get_value(settings->data.sensitivity, calcs_minimum_sensitivity(1, settings->data.period, settings->data.frequency), 255, 0, 0);
  return 0;
}

uint8_t ui_setup_frequency(struct BoilPowerSettings *settings)
{
  settings->data.frequency = ui_get_value(settings->data.frequency, 1, 255, 0, 0);
  settings->data.sensitivity = calcs_minimum_sensitivity(settings->data.sensitivity, settings->data.period, settings->data.frequency);
  return 0;
}

uint8_t ui_setup_user1(struct BoilPowerSettings *settings)
{
  settings->data.userSetpoint[0] = ui_get_value(settings->data.userSetpoint[0], 0, calcs_range(settings->data.period, settings->data.frequency, settings->data.sensitivity), 1, calcs_pwm_percent);
  return 0;
}

uint8_t ui_setup_user2(struct BoilPowerSettings *settings)
{
  settings->data.userSetpoint[1] = ui_get_value(settings->data.userSetpoint[1], 0, calcs_range(settings->data.period, settings->data.frequency, settings->data.sensitivity), 1, calcs_pwm_percent);
  return 0;
}

uint8_t ui_setup_user3(struct BoilPowerSettings *settings)
{
  settings->data.userSetpoint[2] = ui_get_value(settings->data.userSetpoint[2], 0, calcs_range(settings->data.period, settings->data.frequency, settings->data.sensitivity), 1, calcs_pwm_percent);
  return 0;
}

uint8_t ui_setup_hotlock(struct BoilPowerSettings *settings)
{
  settings->data.hotLock = ui_get_yes_no(settings->data.hotLock, " ON", "OFF");
  return 0;
}

uint8_t ui_setup_reset(struct BoilPowerSettings *settings)
{
  if(ui_get_yes_no(0, "yES", " No")) {
    settings->header.size = 0; //Invalidate header
    settings_init(settings); //Initialize Settings
  }
  return 0;
}

uint8_t ui_setup_save(struct BoilPowerSettings *settings)
{
  //Flag for Exit, Settings saved in main() initialization
  return 1;
}

uint16_t ui_get_value(uint16_t value, uint8_t minValue, uint8_t maxValue, uint8_t decimalPosition, uint16_t (*calc_function)(uint8_t, uint8_t))
{
  encoder_set_limits(minValue, maxValue);
  encoder_set_value(value);
  uint16_t displayValue = 0, workingValue = 0;
  uint8_t updateRequired = 1;
  
  while (1) {
    if (encoder_changed())
      updateRequired = 1;
    if (updateRequired) {
      workingValue = encoder_value();
      displayValue = calc_function ? (*calc_function)(workingValue, maxValue) : workingValue;
      if (displayValue > DISPLAY_MAX_NUMBER)
        display_write_string(" ON");
      else
        display_write_number(displayValue, decimalPosition);
      updateRequired = 0;
    }
    if(encoder_ok())
      return workingValue;
    if(encoder_cancel())
      return value;
  }
}

uint8_t ui_get_yes_no(uint8_t value, char displayYes[], char displayNo[])
{
  encoder_set_limits(0, 1);
  encoder_set_value(value ? 1 : 0);
  uint8_t updateRequired = 1;
  
  while (1) {
    if (encoder_changed())
      updateRequired = 1;
    if (updateRequired) {
      if(encoder_value())
        display_write_string(displayYes);
      else
        display_write_string(displayNo);
      updateRequired = 0;
    }
    if(encoder_ok())
      return encoder_value();
    if(encoder_cancel())
      return 0;
  }
}