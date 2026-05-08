#include "Button.h"
#include <Arduino.h>

void Button::begin(uint8_t pin) {
  pin_ = pin;
  pinMode(pin_, INPUT_PULLUP);

  currentState_ = HIGH;
  lastState_ = HIGH;
}

void Button::update() {
  lastState_ = currentState_;
  currentState_ = digitalRead(pin_);
}

bool Button::isPressed() { return currentState_ == LOW; }

bool Button::wasPressed() {
  return (lastState_ == HIGH && currentState_ == LOW);
}

bool Button::wasReleased() {
  return (lastState_ == LOW && currentState_ == HIGH);
}
