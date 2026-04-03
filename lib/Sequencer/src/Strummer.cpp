#include "Strummer.h"
#include "esp32-hal.h"

Strummer::Strummer(Clock &clock, MidiController &midi)
    : clock_(clock), midi_(midi) {}

void Strummer::update() {
  if (this->pos_ >= this->chord_.size) {
    return;
  }
  uint32_t currTime = micros();
  // if current time is >= lastTime + 1 beat : send next note
  if (currTime >= this->lastTime_ + this->clock_.beatsToMicros(1.0f)) {
    this->lastTime_ = micros();
    this->midi_.processNoteOn(this->chord_[this->pos_++]);
  };
}

void Strummer::setChord(Chord chord) {
  this->chord_ = chord;
  this->pos_ = 0;
}

void Strummer::setStrumAmt(uint16_t amount) {
  this->strumAmt_ = amount;
}
