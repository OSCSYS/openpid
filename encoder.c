#include "encoder.h"

#include <avr/interrupt.h>
#include <util/atomic.h> 

#include "display.h" //Provides millis()
#include "hwprofile.h"

static const uint16_t kEncoderOKDuration = 50;
static const uint16_t kEncoderCancelDuration = 1000;

//Global Encoder Variables
static volatile int gEncoderValue = 0;
static volatile uint8_t gEncoderMin = 0;
static volatile uint8_t gEncoderMax = 0;
static volatile uint8_t gEncoderChanged = 0;
static volatile uint32_t gEncoderEnterStartTime = 0;
static volatile enum EncoderEnterState gEncoderEnterState = kEncoderEnterStateIdle;
static volatile uint8_t gEncoderLastBits = 0;

void encoder_init(void)
{
  //Set pin directions
  ENCODER_DIR_REG &= ~kEncoderPinMask;
  
  //Enable Encoder Pin Change Interrupt
  PCICR |= kEncoderPCINTPort;
  
  //Set Pin Change Interrupt Mask for EncA and Enter
  ENCODER_PCINT_MASK_REG |= kEncoderPCINTMask;
  
  //Save pin states for change logic
  gEncoderLastBits = ENCODER_INPUT_REG;
  
  //  Enable global interrupts 
  sei();
}

void encoder_set_limits(uint8_t minimum, uint8_t maximum)
{
  gEncoderMin = minimum;
  gEncoderMax = maximum;
  //Reset value to ensure within limits
  encoder_set_value(encoder_value());
}

uint8_t encoder_minimum()
{
  return gEncoderMin;
}

uint8_t encoder_maximum()
{
  return gEncoderMax;
}

int encoder_value(void)
{
  gEncoderChanged = 0;
  return gEncoderValue;
}

uint8_t encoder_changed(void)
{
  return gEncoderChanged;
}

void encoder_set_value(int value)
{
  value = value > gEncoderMax ? gEncoderMax : value;
  value = value < gEncoderMin ? gEncoderMin : value;
  gEncoderValue = value;
}

uint8_t encoder_ok(void)
{
  if(gEncoderEnterState==kEncoderEnterStateOK) {
    gEncoderEnterState = kEncoderEnterStateIdle;
    return 1;
  }
  return 0;
}

uint8_t encoder_cancel(void)
{
  uint32_t timestamp = millis();
  uint32_t enterStart;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { 
    enterStart = gEncoderEnterStartTime;
  }
  cli();
  if((gEncoderEnterState==kEncoderEnterStateCancel) || ((gEncoderEnterState==kEncoderEnterStateClicked) && (enterStart + kEncoderCancelDuration < timestamp))) {
    gEncoderEnterState = kEncoderEnterStateIdle;
    sei();
    return 1;
  }
  sei();
  return 0;
}

uint8_t encoder_raw_enter(void)
{
  return !(ENCODER_INPUT_REG & kEncoderPinE);
}

ISR(ENCODER_PCINT_VECTOR) 
{
  cli();
  uint8_t encoderBits = ENCODER_INPUT_REG;
  uint8_t encoderChangedBits = gEncoderLastBits ^ encoderBits;

  //Process Encoder A Pin Rising
  if ((encoderChangedBits & kEncoderPinA) && (encoderBits & kEncoderPinA)) {
    if(encoderBits & kEncoderPinB) {
      //EncB is high, Counter-clockwise
      if (gEncoderValue > gEncoderMin)
          --gEncoderValue;
    } else {
      //EncB is low, Clockwise
      if (gEncoderValue < gEncoderMax)
        ++gEncoderValue;
    }
    gEncoderChanged = 1; //Flag value as changed
  }

  //Process Enter Pin Change
  if (encoderChangedBits & kEncoderPinE) {
    switch (gEncoderEnterState) {
      case kEncoderEnterStateIdle:
        //Button is pushed (ActiveLow)
        if (!(encoderBits & kEncoderPinE)) {    
          gEncoderEnterState = kEncoderEnterStateClicked;
          gEncoderEnterStartTime = millis();
        }
        break;
      case kEncoderEnterStateClicked:
        {
          //Assumes interrupt must be enter release
          uint16_t clickDuration = millis() - gEncoderEnterStartTime;
          if(clickDuration < kEncoderOKDuration) {
            gEncoderEnterState = kEncoderEnterStateIdle;
            break;
          }
          if (clickDuration < kEncoderCancelDuration) {
            gEncoderEnterState = kEncoderEnterStateOK;
            break;
          }
          gEncoderEnterState = kEncoderEnterStateCancel;
        }
      default:
        //Events in OK/Cancel state ignored
        break;
    }
  }
  gEncoderLastBits = encoderBits;
  sei();
}