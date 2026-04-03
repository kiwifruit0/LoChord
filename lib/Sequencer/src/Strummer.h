#pragma once
#include "Clock.h"
#include "Theory.h"
#include <cstdint>

class Strummer {
public:
  Strummer(Clock &clock);

  bool shouldPlayNow();

  void setChord(const Chord &chord);

  void clear();

  void setStrumAmt(uint16_t amount);

  uint8_t getNextNoteNum();

private:
  Chord chord_;
  size_t pos_;
  uint16_t strumAmt_;
  uint32_t lastTime_;
  Clock &clock_;
};
