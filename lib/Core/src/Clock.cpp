#include "Clock.h"

Clock::Clock(int bpm) : _bpm(bpm) {}

void Clock::setBpm(int bpm) { _bpm = bpm; }

int Clock::getBpm() const { return _bpm; }

uint8_t Clock::beatsToMicros(float beats) const {
  // microseconds per beat = 60,000,000 / BPM
  return static_cast<uint8_t>(beats * 60000000.0f / _bpm);
}

float Clock::microsToBeats(uint8_t micros) const {
  return static_cast<float>(micros) * _bpm / 60000000.0f;
}

uint8_t Clock::subdivisionMicros(float subdivision) const {
  return beatsToMicros(subdivision);
}
