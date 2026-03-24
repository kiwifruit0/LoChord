#include "ButtonCtrl.h"

void ButtonCtrl::begin(int pin) {
  _pin = pin;
  pinMode(_pin, INPUT_PULLUP);

  _currentState = HIGH;
  _lastState = HIGH;
}

void ButtonCtrl::update() {
  _lastState = _currentState;
  _currentState = digitalRead(_pin);
}

bool ButtonCtrl::isPressed() { return _currentState == LOW; }

bool ButtonCtrl::wasPressed() {
  return (_lastState == HIGH && _currentState == LOW);
}

bool ButtonCtrl::wasReleased() {
  return (_lastState == LOW && _currentState == HIGH);
}
