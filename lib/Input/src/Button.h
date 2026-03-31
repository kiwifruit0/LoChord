#pragma once
#include <Arduino.h>

class Button {
public:
  void begin(int pin);
  void update();

  bool isPressed();
  bool wasPressed();
  bool wasReleased();

private:
  int pin_;
  bool currentState_;
  bool lastState_;
};
