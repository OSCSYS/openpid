#ifndef BOILPOWER_UI_H_
#define BOILPOWER_UI_H_

#include "settings.h"

void ui_setup(struct BoilPowerSettings *settings);
void ui_init(struct BoilPowerSettings *settings);
void ui_update(void);

#endif