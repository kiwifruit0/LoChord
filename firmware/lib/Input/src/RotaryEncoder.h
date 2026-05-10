#pragma once
#include "Button.h"
#include <cstdint>
#include <esp_attr.h>

class RotaryEncoder {
public:
  void begin(uint8_t pinA, uint8_t pinB, uint8_t pinButton);
  void update();
  int getDelta();
  Button &getButton() { return button_; }

private:
  int pinA_;
  int pinB_;

  Button button_;

  volatile int counter_ = 0;
  int lastCounter_ = 0;

  static RotaryEncoder *instance_;
  static void IRAM_ATTR isrTrampoline();
  void IRAM_ATTR handleIsr();
};
