#ifndef BOILPOWER_ENCODER_H_
#define BOILPOWER_ENCODER_H_

#include <stdint.h>
#include <avr/io.h> 

enum EncoderEnterState {
  kEncoderEnterStateIdle,
  kEncoderEnterStateClicked,
  kEncoderEnterStateOK,
  kEncoderEnterStateCancel
};
  
//Function declarations
void encoder_init(void);
void encoder_set_limits(uint8_t minimum, uint8_t maximum);
uint8_t encoder_minimum(void);
uint8_t encoder_maximum(void);
int encoder_value(void);
uint8_t encoder_changed(void);
void encoder_set_value(int value);
uint8_t encoder_ok(void);
uint8_t encoder_cancel(void);
uint8_t encoder_raw_enter(void);

#endif