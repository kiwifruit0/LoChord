#pragma once

#include "Button.h"
#include <cstdint>

class Joystick {
public:
  void begin(uint8_t udPin, uint8_t lrPin, uint8_t buttonPin);
  void update();

  bool wasChanged();

  Button &getButton() { return this->button_; };

  uint8_t getCurrentPos();

private:
  uint8_t udPin_;
  uint8_t lrPin_;

  // stores position from 1-9
  uint8_t currentPos_;
  uint8_t lastPos_;

  uint32_t timeChanged_;

  Button button_;

  // returns a value from 0-8 representing the joystick position
  void setCurrentPos();
};
