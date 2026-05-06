#pragma once
#include <Arduino.h>
#include "Display.h"

class UIController {
public:
  explicit UIController(Display& display);
  void buttonPressed(uint8_t buttonId);

private:
  Display& display_;
};
