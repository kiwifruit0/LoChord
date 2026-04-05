#include "MidiController.h"
#include <Arduino.h>
#include <cstdlib>

MidiController::MidiController(ChordGenerator chordGen, Clock &clock,
                               MidiOutput &output, bool chordMode, bool strumOn,
                               bool arpOn, float defaultVelocity,
                               float randVelocityAmt)
    : output_(output), clock_(clock), strummer_(clock), chordGen_(chordGen),
      chordMode_(chordMode), strumOn_(strumOn), arpOn_(arpOn),
      defaultVelocity_(defaultVelocity), randVelocityAmt_(randVelocityAmt) {}

void MidiController::processNoteOn(int root, int joystickPos) {
  if (chordMode_) {
    processNoteOff();
    // send chord
    activeChord_ = chordGen_.getMidiChord(root, joystickPos);
    sendChord(activeChord_);
  } else {
    // send single note
    activeChord_.clear();
    activeChord_.addNote(chordGen_.getNoteNum(root));
    sendNote(activeChord_[0]);
  }
}

void MidiController::processNoteOff() {
  // send note off for all notes in the active chord
  for (size_t i = 0; i < activeChord_.size; i++) {
    output_.noteOff(activeChord_[i]);
  }
  activeChord_.clear();
  strummer_.clear();
}

void MidiController::setChordMode(bool enabled) { chordMode_ = enabled; }

void MidiController::setStrumOn(bool strumOn) {
  strumOn_ = strumOn;
  if (strumOn) {
    arpOn_ = 0;
  }
}

void MidiController::setArpOn(bool arpOn) {
  strumOn_ = arpOn;
  if (arpOn) {
    strumOn_ = 0;
  }
}

void MidiController::setVelocity(float velocity) {
  defaultVelocity_ = velocity;
}

ChordGenerator &MidiController::getChordGenerator() { return chordGen_; }

uint8_t MidiController::calculateVelocity() {
  // calculates random velocity as default - random in range(0, default * random
  // amount) e.g. if default = 100 and random amount = 0.1: calculated velocity
  // is between 90 and 100
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
  if (strumOn_) {
    strummer_.setChord(chord);
  } else if (arpOn_) {
    // todo: arp
  } else {
    for (size_t i = 0; i < chord.size; i++) {
      sendNote(chord[i]);
    }
  }
}

void MidiController::update() {
  if (strumOn_ && strummer_.shouldPlayNow()) {
    sendNote(strummer_.getNextNoteNum());
  }
}

Strummer MidiController::getStrummer() { return strummer_; }
