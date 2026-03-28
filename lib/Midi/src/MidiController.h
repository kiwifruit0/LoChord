#pragma once
#include <USBMIDI.h>

class MidiController {
public:
  USBMIDI getMidiDevice();

private:
  USBMIDI _midi;
};
