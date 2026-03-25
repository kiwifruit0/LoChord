#include "Button.h"

void Button::begin(int pin) {
  _pin = pin;
  pinMode(_pin, INPUT_PULLUP);

  _currentState = HIGH;
  _lastState = HIGH;
}

void Button::update() {
  _lastState = _currentState;
  _currentState = digitalRead(_pin);
}

bool Button::isPressed() { return _currentState == LOW; }

bool Button::wasPressed() {
  return (_lastState == HIGH && _currentState == LOW);
}

bool Button::wasReleased() {
  return (_lastState == LOW && _currentState == HIGH);
}
