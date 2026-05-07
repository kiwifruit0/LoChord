#pragma once
#include "Display.h"
#include <Arduino.h>

class UIController {
public:
  explicit UIController(Display &display);

  void setChordRoot(const char *rootText);

private:
  Display &display_;
};
