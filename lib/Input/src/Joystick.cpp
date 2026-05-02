#include "Joystick.h"
#include <Arduino.h>

void Joystick::begin(uint8_t pin) { this->pin_ = pin; }

bool Joystick::wasChanged() { return currentPos_ != lastPos_; }

uint8_t Joystick::getPos() {
  int rawValue = analogRead(pin_);
  lastPos_ = currentPos_;
  currentPos_ = rawValue / 410;
  return currentPos_; // map 0-4096 to 0-9
}

void Joystick::update() { getPos(); }
