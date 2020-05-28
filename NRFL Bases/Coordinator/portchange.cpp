#include "portchange.h"

struct pinchange_t {
  volatile uint8_t values;
  uint8_t conditions[8 / 4];
  interrupt_cb callbacks[8];
};

static pinchange_t *p0 = NULL, *p1 = NULL, *p2 = NULL;

ISR(PCINT0_vect) {
  if (p0) {
    uint8_t values = PINB;

    for (uint8_t i = 0; i < 6; ++i) {
      if (p0->callbacks[i] && (((values >> i) & 0x01) != ((p0->values >> i) & 0x01))) {
        uint8_t condition = (p0->conditions[i / 4] >> (i % 4) * 2) & PC_ANY;

        if (((condition & PC_RISING) && (((values >> i) & 0x01) == 1)) || ((condition & PC_FALLING) && (((values >> i) & 0x01) == 0)))
          p0->callbacks[i]((values >> i) & 0x01);
      }
    }
    p0->values = values;
  }
}

ISR(PCINT1_vect) {
  if (p1) {
    uint8_t values = PINC;

    for (uint8_t i = 0; i < 6; ++i) {
      if (p1->callbacks[i] && (((values >> i) & 0x01) != ((p1->values >> i) & 0x01))) {
        uint8_t condition = (p1->conditions[i / 4] >> (i % 4) * 2) & PC_ANY;

        if (((condition & PC_RISING) && (((values >> i) & 0x01) == 1)) || ((condition & PC_FALLING) && (((values >> i) & 0x01) == 0)))
          p1->callbacks[i]((values >> i) & 0x01);
      }
    }
    p1->values = values;
  }
}

ISR(PCINT2_vect) {
  if (p2) {
    uint8_t values = PIND;

    for (uint8_t i = 0; i < 8; ++i) {
      if (p2->callbacks[i] && (((values >> i) & 0x01) != ((p2->values >> i) & 0x01))) {
        uint8_t condition = (p2->conditions[i / 4] >> (i % 4) * 2) & PC_ANY;

        if (((condition & PC_RISING) && (((values >> i) & 0x01) == 1)) || ((condition & PC_FALLING) && (((values >> i) & 0x01) == 0)))
          p2->callbacks[i]((values >> i) & 0x01);
      }
    }
    p2->values = values;
  }
}

void attachInterruptEx(uint8_t pin, interrupt_cb cb, uint8_t mode) {
  if (pin < 8) { // D0..D7
    if (! p2) {
      p2 = (pinchange_t*)malloc(sizeof(pinchange_t));
      memset(p2, 0, sizeof(pinchange_t));
      p2->values = PIND;
    }
    p2->conditions[pin / 4] &= ~(uint8_t)(PC_ANY << (pin % 4) * 2);
    p2->conditions[pin / 4] |= ((mode & PC_ANY) << (pin % 4) * 2);
    p2->callbacks[pin] = cb;
    PCIFR |= (1 << PCIF2);
    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << pin);
  } else if (pin < A0) { // D8..D13
    pin -= 8;
    if (! p0) {
      p0 = (pinchange_t*)malloc(sizeof(pinchange_t) - sizeof(interrupt_cb) * 2);
      memset(p0, 0, sizeof(pinchange_t) - sizeof(interrupt_cb) * 2);
      p0->values = PINB;
    }
    p0->conditions[pin / 4] &= ~(uint8_t)(PC_ANY << (pin % 4) * 2);
    p0->conditions[pin / 4] |= ((mode & PC_ANY) << (pin % 4) * 2);
    p0->callbacks[pin] = cb;
    PCIFR |= (1 << PCIF0);
    PCICR |= (1 << PCIE0);
    PCMSK0 |= (1 << pin);
  } else if (pin <= A5) { // A0..A5
    pin -= A0;
    if (! p1) {
      p1 = (pinchange_t*)malloc(sizeof(pinchange_t) - sizeof(interrupt_cb) * 2);
      memset(p1, 0, sizeof(pinchange_t) - sizeof(interrupt_cb) * 2);
      p1->values = PINC;
    }
    p1->conditions[pin / 4] &= ~(uint8_t)(PC_ANY << (pin % 4) * 2);
    p1->conditions[pin / 4] |= ((mode & PC_ANY) << (pin % 4) * 2);
    p1->callbacks[pin] = cb;
    PCIFR |= (1 << PCIF1);
    PCICR |= (1 << PCIE1);
    PCMSK1 |= (1 << pin);
  }
}

static bool isEmpty(const pinchange_t *pc, uint8_t len = 8) {
  for (uint8_t i = 0; i < len; ++i) {
    if (pc->callbacks[i])
      return false;
  }

  return true;
}

void detachInterruptEx(uint8_t pin) {
  if (pin < 8) { // D0..D7
    if (p2) {
      p2->conditions[pin / 4] &= ~(uint8_t)(PC_ANY << (pin % 4) * 2);
      p2->callbacks[pin] = NULL;
      PCMSK2 &= ~(uint8_t)(1 << pin);
      if (isEmpty(p2)) {
        PCICR &= ~(uint8_t)(1 << PCIE2);
        free(p2);
        p2 = NULL;
      }
    }
  } else if (pin < A0) { // D8..D13
    pin -= 8;
    if (p0) {
      p0->conditions[pin / 4] &= ~(uint8_t)(PC_ANY << (pin % 4) * 2);
      p0->callbacks[pin] = NULL;
      PCMSK0 &= ~(uint8_t)(1 << pin);
      if (isEmpty(p0, 6)) {
        PCICR &= ~(uint8_t)(1 << PCIE0);
        free(p0);
        p0 = NULL;
      }
    }
  } else if (pin <= A5) { // A0..A5
    pin -= A0;
    if (p1) {
      p1->conditions[pin / 4] &= ~(uint8_t)(PC_ANY << (pin % 4) * 2);
      p1->callbacks[pin] = NULL;
      PCMSK1 &= ~(uint8_t)(1 << pin);
      if (isEmpty(p1, 6)) {
        PCICR &= ~(uint8_t)(1 << PCIE1);
        free(p1);
        p1 = NULL;
      }
    }
  }
}
