#include "Joystick.h"
#include <Arduino.h>

void Joystick::begin(uint8_t pin) { this->pin_ = pin; }

bool Joystick::wasChanged() { return false; }

uint8_t Joystick::getPos() {
  int rawValue = analogRead(pin_);
  return rawValue / 410; // map 0-4096 to 0-9
}
