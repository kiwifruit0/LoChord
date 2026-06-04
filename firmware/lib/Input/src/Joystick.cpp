#include "Joystick.h"
#include <Arduino.h>

void Joystick::begin(uint8_t udPin, uint8_t lrPin, uint8_t buttonPin) {
  this->udPin_ = udPin;
  this->lrPin_ = lrPin;

  this->button_ = Button();
  this->button_.begin(buttonPin);

  currentPos_ = 0;
  lastPos_ = 0;
  getPos();
  lastPos_ = currentPos_;
}

bool Joystick::wasChanged() { return currentPos_ != lastPos_; }

uint8_t Joystick::getPos() {
  // maps inputs to 0 - 2
  int xVal = map(analogRead(lrPin_), 0, 4096, 0, 2);
  int yVal = map(analogRead(udPin_), 0, 4096, 0, 2);

  // returns output so top left is 0, top middle is 1, centre is 4 etc
  // to match up with the order of the chord qualities in Theory.h
  lastPos_ = currentPos_;
  currentPos_ = xVal + 3 * yVal;
  return currentPos_;
}

void Joystick::update() {
  button_.update();
  getPos();
}
