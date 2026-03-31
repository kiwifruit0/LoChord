#include "MidiController.h"
#include <Arduino.h>
#include <cstdlib>

MidiController::MidiController(ChordGenerator chordGen, Clock &clock,
                               MidiOutput &output, bool chordMode,
                               float defaultVelocity, float strumAmt,
                               float randVelocityAmt)
    : output_(output), clock_(clock), chordGen_(chordGen),
      chordMode_(chordMode), strumAmt_(strumAmt),
      randVelocityAmt_(randVelocityAmt), defaultVelocity_(defaultVelocity) {}

void MidiController::processNoteOn(int root) {
  if (chordMode_) {
    activeChord_ = chordGen_.getMidiChord(root);
    sendChord(activeChord_);
  } else {
    activeChord_.clear();
    activeChord_.addNote(chordGen_.getNoteNum(root));
    sendNote(activeChord_[0]);
  }
}

void MidiController::processNoteOff(int root) {
  // Send note-off for all notes in the active chord
  for (size_t i = 0; i < activeChord_.size; i++) {
    output_.noteOff(activeChord_[i]);
  }
  activeChord_.clear();
}

void MidiController::setChordMode(bool enabled) { chordMode_ = enabled; }

void MidiController::setStrumAmount(float amount) { strumAmt_ = amount; }

void MidiController::setVelocity(float velocity) {
  defaultVelocity_ = velocity;
}
ChordGenerator &MidiController::getChordGenerator() { return chordGen_; }

uint8_t MidiController::calculateVelocity() {
  if (randVelocityAmt_ <= 0) {
    return static_cast<uint8_t>(defaultVelocity_);
  }
  int maxRandom = static_cast<int>(defaultVelocity_ * randVelocityAmt_);
  int randomOffset = (maxRandom > 0) ? (rand() % maxRandom) : 0;
  return static_cast<uint8_t>(defaultVelocity_ - randomOffset);
}

void MidiController::sendNote(int noteNum) {
  output_.noteOn(noteNum, calculateVelocity());
}

void MidiController::sendChord(const Chord &chord) {
  // TODO: Replace with Scheduler-based strum for non-blocking playback
  // currently plays all notes simultaneously (strum disabled)
  for (size_t i = 0; i < chord.size; i++) {
    sendNote(chord.notes[i]);
  }
}
