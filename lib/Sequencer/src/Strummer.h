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

  void setStrumDenom(uint8_t denom);
  uint8_t getStrumDenom();

  uint8_t getNextNoteNum();

  void setStrumMode(char8_t mode);

  char8_t getStrumMode();

private:
  Chord chord_;
  size_t pos_;

  // amount of strum
  // e.g. strumDenom = 4: every note played 1/4ths apart
  uint8_t strumDenom_ = 8;
  uint32_t lastTime_;
  Clock &clock_;

  // u: up, d: down, r: random
  char8_t strumMode_ = 'u';

  void randomiseChordNotes();
};
