#pragma once
#include <cstdint>

class Clock {
public:
  explicit Clock(int bpm = 120);

  void setBpm(int bpm);
  int getBpm() const;

  // Time conversions
  uint8_t beatsToMicros(float beats) const;
  float microsToBeats(uint8_t micros) const;

  // Get interval for a note subdivision (1.0 = quarter, 0.5 = eighth, etc.)
  uint8_t subdivisionMicros(float subdivision) const;

private:
  int _bpm;
};
