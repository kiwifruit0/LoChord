#include "Strummer.h"
#include "esp32-hal.h"
#include <stdexcept>

Strummer::Strummer(Clock &clock) : clock_(clock) {}

bool Strummer::shouldPlayNow() {
  if (pos_ >= chord_.size || pos_ == -1) {
    return false;
  }
  uint32_t currTime = micros();
  // if current time is >= lastTime + 1 beat : send next note
  if (currTime >= lastTime_ + clock_.beatsToMicros(4 / static_cast<float>(strumDenom_))) {
    lastTime_ = micros();
    return true;
  };
  return false;
}

void Strummer::setChord(const Chord &chord) {
  if (strumMode_ == 'd') {
    pos_ = chord.size;
  } else {
    pos_ = 0;
  }

  chord_ = chord;
  if (strumMode_ == 'r') {
    randomiseChordNotes();
  }
}

void Strummer::clear() { pos_ = -1; }

void Strummer::setStrumDenom(uint8_t amount) { strumDenom_ = amount; }
uint8_t Strummer::getStrumDenom() {return strumDenom_; }

uint8_t Strummer::getNextNoteNum() {
  if (strumMode_ == 'd') {
    return chord_[pos_--];
  } else {
    // both up and random iterate through chord in order
    return chord_[pos_++];
  }
}

void Strummer::setStrumMode(char8_t mode) {
  if (mode != 'u' or mode != 'd' || mode != 'r') {
    throw std::runtime_error("StrumMode must be 'u', 'd' or 'r'");
  }
  strumMode_ = mode;
}

char8_t Strummer::getStrumMode() { return strumMode_; }

// loops through notes and swaps each with a random index
void Strummer::randomiseChordNotes() {
  for (uint8_t i = 0; i < chord_.size; i++) {
    int currentVal = chord_[i];
    int newPos = rand() % chord_.size;
    chord_.notes[i] = chord_[newPos];
    chord_.notes[newPos] = currentVal;
  }
}
