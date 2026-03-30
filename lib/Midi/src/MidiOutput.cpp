#include "MidiOutput.h"

void MidiOutput::begin() { _midi.begin(); }

void MidiOutput::send(const MidiEvent &event) {
  switch (event.type) {
  case MidiEventType::NoteOn:
    noteOn(event.note, event.velocity, event.channel);
    break;
  case MidiEventType::NoteOff:
    noteOff(event.note, event.channel);
    break;
  case MidiEventType::ControlChange:
    // Future: _midi.controlChange(...)
    break;
  }
}

void MidiOutput::noteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
  _midi.noteOn(note, velocity, channel);
}

void MidiOutput::noteOff(uint8_t note, uint8_t channel) {
  _midi.noteOff(note, 0, channel);
}

USBMIDI &MidiOutput::getRawMidi() { return _midi; }
