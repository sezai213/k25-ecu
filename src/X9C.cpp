#include "X9C.h"

X9C::X9C(uint8_t pinCS, uint8_t pinINC, uint8_t pinUD)
    : _cs(pinCS), _inc(pinINC), _ud(pinUD), _currentValue(0) {}

void X9C::begin() {
  pinMode(_cs, OUTPUT);
  pinMode(_inc, OUTPUT);
  pinMode(_ud, OUTPUT);

  digitalWrite(_cs, HIGH);
  digitalWrite(_inc, HIGH);
  digitalWrite(_ud, LOW);
}

void X9C::setValue(uint8_t target) {
  if (target > 99) target = 99;

  if (target == _currentValue) return;

  digitalWrite(_cs, LOW);
  delayMicroseconds(2);

  if (target == 0) {
    digitalWrite(_ud, LOW);
    for (int i = 0; i < 100; i++) pulseINC();
    _currentValue = 0;
  } else if (target > _currentValue) {
    digitalWrite(_ud, HIGH);
    for (int i = 0; i < target - _currentValue; i++) pulseINC();
    _currentValue = target;
  } else {
    digitalWrite(_ud, LOW);
    for (int i = 0; i < _currentValue - target; i++) pulseINC();
    _currentValue = target;
  }

  digitalWrite(_cs, HIGH);
  delayMicroseconds(5);
}

uint8_t X9C::getCurrentValue() const {
  return _currentValue;
}

void X9C::pulseINC() {
  digitalWrite(_inc, LOW);
  delayMicroseconds(5);
  digitalWrite(_inc, HIGH);
  delayMicroseconds(5);
}