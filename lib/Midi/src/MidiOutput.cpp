#include "MidiOutput.h"

void MidiOutput::begin() { midi_.begin(); }

void MidiOutput::noteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
  midi_.noteOn(note, velocity, channel);
}

void MidiOutput::noteOff(uint8_t note, uint8_t channel) {
  midi_.noteOff(note, 0, channel);
}

USBMIDI &MidiOutput::getRawMidi() { return midi_; }
