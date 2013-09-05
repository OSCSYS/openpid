#ifndef BOILPOWER_STATUS_H_
#define BOILPOWER_STATUS_H_

#include <stdint.h>
#include <avr/io.h> 
 
//Status port bit mapping
static const uint8_t kStatusHeat   = _BV(0);
static const uint8_t kStatusLock = _BV(1);
static const uint8_t kStatusDebug = _BV(5);
 
//Function Declarations
void status_init(void);
void status_set(uint8_t status_mode);
void status_clear(uint8_t status_mode);
void status_toggle(uint8_t status_mode);

#endif