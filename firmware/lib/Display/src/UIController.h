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

  void updateJoystickMatrix(uint8_t joystickPos,
                            const std::array<const char *, 12> &map);

  void clearJoystickMatrix();

private:
  Display &display_;

  bool lastMidiReady_ = false;
};
