#pragma once
#include <USBMIDI.h>

class MidiController {
public:
  MidiController(bool chordMode = true, float defaultVelocity = 100,
                 float strumAmt = 0, float randVelocityAmt = 0);

  USBMIDI getMidiDevice();

private:
  USBMIDI midi;
  bool chordMode;
  float strumAmt;
  float randVelocityAmt;
  float defaultVelocity;
};
