#pragma once
#include "Clock.h"
#include "MidiController.h"
#include "Theory.h"
#include <cstdint>

class Strummer {
public:
  Strummer(Clock &clock, MidiController &midi);
  void update();

  void clear();

  void setChord(Chord chord);

  void setStrumAmt(uint16_t amount);

private:
  Chord chord_;
  size_t pos_;
  uint16_t strumAmt_;
  uint32_t lastTime_;
  Clock &clock_;
  MidiController &midi_;
};
