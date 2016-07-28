#ifndef BOILPOWER_HWPROFILE_H_
#define BOILPOWER_HWPROFILE_H_

#include <avr/io.h> 

//Display output registers
#define DISPLAY_CHAR_SELECT_OUTPUT_REG PORTC
#define DISPLAY_CHAR_OUTPUT_REG        PORTD

//Display direction registers
#define DISPLAY_CHAR_SELECT_DIR_REG DDRC
#define DISPLAY_CHAR_DIR_REG         DDRD

//Display pin bitmasks
static const uint8_t kDisplayCharSelectPinMask  = 0x38;
static const uint8_t kDisplayCharPinMask         = 0xff;

//Display char select bit map
#define DISPLAY_CHAR_COUNT 3
#define DISPLAY_MAX_NUMBER 999
static const uint8_t kDisplayCharSelect[DISPLAY_CHAR_COUNT] = {_BV(5), _BV(4), _BV(3)};

//Display Timer Configuration
#define DISPLAY_TIMER_CONFIG_A_REG        TCCR0A
#define DISPLAY_TIMER_CONFIG_B_REG        TCCR0B
#define DISPLAY_TIMER_INTERRUPT_MASK_REG  TIMSK0
#define DISPLAY_TIMER_COMPARE_VALUE_REG   OCR0A

static const uint8_t kDisplayTimerMode = _BV(WGM01);
static const uint8_t kDisplayTimerInterruptMask = _BV(OCIE0A);
static const uint8_t kDisplayTimerCompareValue = 125;
static const uint8_t kDisplayTimerPrescaler = (_BV(CS00) | _BV(CS01));




//Encoder input register
#define ENCODER_INPUT_REG      PINC

//Encoder direction register
#define ENCODER_DIR_REG      DDRC

//Encoder pin bitmask
static const uint8_t kEncoderPinMask      = 0x07;

//Encoder pin function mapping
static const uint8_t kEncoderPinA = _BV(0);
static const uint8_t kEncoderPinB = _BV(1);
static const uint8_t kEncoderPinE = _BV(2);

//Encoder Pin Change Interrupts
#define ENCODER_PCINT_MASK_REG PCMSK1
#define ENCODER_PCINT_VECTOR PCINT1_vect
static const uint8_t kEncoderPCINTPort = _BV(PCIE1);
static const uint8_t kEncoderPCINTMask = _BV(PCINT8) | _BV(PCINT10);



//Status output register
#define STATUS_OUTPUT_REG PORTB

//Status direction register
#define STATUS_DIR_REG DDRB

//Status pin bitmask
static const uint8_t kStatusPinMask      = 0x23;

//Status output register
#define PWM_OUTPUT_REG PORTB


/* PWM Output Pin PB2 */
//PWM direction register
#define PWM_DIR_REG DDRB

//PWM pin bitmask
static const uint8_t kPwmPinMask      = 0x04;


//OneWire PB3
#define ONEWIRE_INPUT_REG PINB
#define ONEWIRE_OUTPUT_REG PORTB
#define ONEWIRE_DIR_REG DDRB
static const uint8_t kOnewirePinMask      = 0x08;

//Option PB4

#endif
