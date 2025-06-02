#ifndef X9C_H
#define X9C_H

#include <Arduino.h>

class X9C {
public:
  X9C(uint8_t pinCS, uint8_t pinINC, uint8_t pinUD);
  void begin();
  void reset();
  void setValue(uint8_t target);
  uint8_t getCurrentValue() const;

private:
  uint8_t _cs, _inc, _ud;
  uint8_t _currentValue;
  void pulseINC();
};

#endif