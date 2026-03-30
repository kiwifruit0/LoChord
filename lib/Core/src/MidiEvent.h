#pragma once
#include <cstdint>

enum class MidiEventType : uint8_t {
  NoteOn,
  NoteOff,
  ControlChange,
};

struct MidiEvent {
  uint32_t timestamp;  // microseconds from reference point (loop start, etc.)
  MidiEventType type;
  uint8_t note;
  uint8_t velocity;
  uint8_t channel;

  static MidiEvent noteOn(uint8_t note, uint8_t velocity, uint8_t channel = 0) {
    return MidiEvent{0, MidiEventType::NoteOn, note, velocity, channel};
  }

  static MidiEvent noteOff(uint8_t note, uint8_t channel = 0) {
    return MidiEvent{0, MidiEventType::NoteOff, note, 0, channel};
  }

  MidiEvent withTimestamp(uint32_t ts) const {
    return MidiEvent{ts, type, note, velocity, channel};
  }
};
