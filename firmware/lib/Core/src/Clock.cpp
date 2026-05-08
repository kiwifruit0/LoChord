#include "Clock.h"

Clock::Clock(int bpm) : bpm_(bpm) {}

void Clock::setBpm(int bpm) { bpm = bpm; }

int Clock::getBpm() const { return bpm_; }

uint32_t Clock::beatsToMicros(float beats) const {
  // microseconds per beat = 60,000,000 / BPM
  return static_cast<uint32_t>(beats * 60000000.0f / bpm_);
}

float Clock::microsToBeats(uint32_t micros) const {
  return static_cast<float>(micros) * bpm_ / 60000000.0f;
}
