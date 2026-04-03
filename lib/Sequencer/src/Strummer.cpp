#include "Strummer.h"
#include "esp32-hal.h"

Strummer::Strummer(Clock &clock)
    : clock_(clock) {}

bool Strummer::shouldPlayNow() {
  if (this->pos_ >= this->chord_.size) {
    return false;
  }
  uint32_t currTime = micros();
  // if current time is >= lastTime + 1 beat : send next note
  if (currTime >= this->lastTime_ + this->clock_.beatsToMicros(1.0f)) {
    this->lastTime_ = micros();
    return true;
  };
  return false;
}

void Strummer::setChord(const Chord &chord) {
  this->chord_ = chord;
  this->pos_ = 0;
}

void Strummer::clear() { this->pos_ = 0; }

void Strummer::setStrumAmt(uint16_t amount) { this->strumAmt_ = amount; }

uint8_t Strummer::getNextNoteNum() { return this->chord_[this->pos_++]; }
