#pragma once

#include <cstdint>

class RotaryEncoder {
public:
  void begin(uint8_t pin);

  void update();

  bool hasChanged();

private:
  int pin_;
  uint8_t currentPos_;
  uint8_t lastPos_;
};
