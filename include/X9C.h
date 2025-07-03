#ifndef X9C_H
#define X9C_H

#include <Arduino.h>
#define X9C_MIN_STEP    0
#define X9C_MAX_STEP    30
class X9C {
public:
  X9C(uint8_t pinCS, uint8_t pinINC, uint8_t pinUD);
  void initialize();
  void reset();
  void setPercentageValue(float target);
  void setStepValue(uint8_t target);
  uint8_t getCurrentValue() const;

private:
  uint8_t _cs, _inc, _ud;
  uint8_t _currentValue;
  void pulseINC();
};

#endif