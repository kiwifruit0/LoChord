#pragma once
#include "Display.h"
#include <Arduino.h>

class UIController {
public:
  explicit UIController(Display &display);

  void updateUsbStatus();

  void setChordRoot(const char *rootText);

  void setChordQuality(const char *qualityText);

  void setChordNotes(const char *notesText);

  void clearJoystickMatrix();

private:
  Display &display_;

  bool lastMidiReady_ = false;
};
