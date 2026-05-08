#pragma once

#include <cstdint>

class Button {
public:
  void begin(uint8_t pin);
  void update();

  bool isPressed();
  bool wasPressed();
  bool wasReleased();

private:
  int pin_;
  bool currentState_;
  bool lastState_;
};
