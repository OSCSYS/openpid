#ifndef BOILPOWER_DISPLAY_H_
#define BOILPOWER_DISPLAY_H_

#include <stdint.h>
#include <avr/io.h> 

//Function declarations
void display_init(void);

//Write a number with static decimal point
//precision: 0 = No decimal point (ie 1), 1 = Char 2 (ie 1.0), 2 = Char 3 (ie 1.00)
void display_write_number(int number, uint8_t precision);

//Write a string (limited to display size, limited char support 0-9, A-U)
void display_write_string(const char* text);

uint32_t millis(void);

#endif
