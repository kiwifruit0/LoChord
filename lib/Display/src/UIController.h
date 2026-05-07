#pragma once
#include "Display.h"
#include <Arduino.h>

class UIController {
public:
  explicit UIController(Display &display);

  void setChordRoot(const char *rootText);

  void setChordQuality(const char *qualityText);

private:
  Display &display_;
};
