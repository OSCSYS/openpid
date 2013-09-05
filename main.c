#include "display.h"
#include "encoder.h"
#include "pwm.h"
#include "settings.h"
#include "status.h"
#include "ui.h"

int main(void)
{
  pwm_init();
  status_init();
  display_init();
  encoder_init();

  struct BoilPowerSettings systemSettings;
  settings_load(&systemSettings);
  
  //Check settings validity launching settings UI Menu if necessary
  if (settings_init(&systemSettings) || encoder_raw_enter()) {
    ui_setup(&systemSettings);
    settings_save(&systemSettings);
  }

  ui_init(&systemSettings);
  pwm_set_period(systemSettings.data.period * 100);
  
  while (1) {
    ui_update();
    pwm_update();
  }
}

