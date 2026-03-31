#pragma once
#include <cstdint>

class Clock {
public:
  explicit Clock(int bpm = 120);

  void setBpm(int bpm);
  int getBpm() const;

  // time conversions
  uint32_t beatsToMicros(float beats) const;
  float microsToBeats(uint32_t micros) const;

  // get interval for a note subdivision (e.g. 1.0 = quarter)
  uint32_t subdivisionMicros(float subdivision) const;

private:
  int bpm_;
};
