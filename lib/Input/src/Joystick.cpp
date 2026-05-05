#include "Joystick.h"
#include <Arduino.h>

void Joystick::begin(uint8_t udPin, uint8_t lrPin, uint8_t buttonPin) {
  this->udPin_ = udPin;
  this->lrPin_ = lrPin;

  this->button_ = Button();
  this->button_.begin(buttonPin);
}

bool Joystick::wasChanged() { return currentPos_ != lastPos_; }

uint8_t Joystick::getPos() {
  int xVal = analogRead(lrPin_);
  int yVal = analogRead(udPin_);
  lastPos_ = currentPos_;
  // currentPos_ = rawValue / 410;
  return xVal; // map 0-4096 to 0-9
}

void Joystick::update() { getPos(); }
