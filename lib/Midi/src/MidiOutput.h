#pragma once
#include "MidiEvent.h"
#include <USBMIDI.h>

class MidiOutput {
public:
  void begin();

  void send(const MidiEvent &event);
  void noteOn(uint8_t note, uint8_t velocity, uint8_t channel = 0);
  void noteOff(uint8_t note, uint8_t channel = 0);

  USBMIDI &getRawMidi();

private:
  USBMIDI _midi;
};
