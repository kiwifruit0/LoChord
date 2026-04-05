#pragma once

#include <cstdint>

class Joystick {
public:
  void begin(uint8_t pin);
  void update();

  bool wasChanged();

  uint8_t getPos();

private:
  int pin_;
  uint8_t currentPos_;
  uint8_t lastPos_;
};
