#pragma once
#include <Arduino.h>

class ButtonCtrl {
public:
  void begin(int pin);
  void update();

  bool isPressed();
  bool wasPressed();
  bool wasReleased();

  private:
    int _pin;
    bool _currentState;
    bool _lastState;
  };
