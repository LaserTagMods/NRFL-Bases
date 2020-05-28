#ifndef __PORTCHANGE_H
#define __PORTCHANGE_H

#include <Arduino.h>

typedef void (*interrupt_cb)(bool pinvalue);

const uint8_t PC_RISING = 0x01;
const uint8_t PC_FALLING = 0x02;
const uint8_t PC_ANY = PC_RISING | PC_FALLING;

void attachInterruptEx(uint8_t pin, interrupt_cb cb, uint8_t mode);
void detachInterruptEx(uint8_t pin);

#endif
