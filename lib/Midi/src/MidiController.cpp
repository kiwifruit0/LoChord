#include "MidiController.h"
#include "Theory.h"
#include <Arduino.h>
#include <cstdlib>

MidiController::MidiController(ChordGenerator chordGen, Clock &clock,
                               MidiOutput &output, bool chordMode, bool strumOn,
                               bool arpOn, float defaultVelocity,
                               float randVelocityAmt)
    : output_(output), clock_(clock), sequencer_(clock), chordGen_(chordGen),
      chordMode_(chordMode), defaultVelocity_(defaultVelocity),
      randVelocityAmt_(randVelocityAmt) {}

void MidiController::processNoteOn(int buttonId, int joystickPos) {
  // stop any currently playing chord
  stopCurrentChord();

  if (chordMode_) {
    // send chord
    auto result = chordGen_.getMidiChord(buttonId, joystickPos);
    activeChord_ = result.chord;
    activeButtonId_ = buttonId;
    sendChord(activeChord_);

    if (ui_) {
      ui_->setChordRoot(toString(activeChord_[0] % 12));
      ui_->setChordQuality(toString(result.quality));
    }

  } else {
    // send single note
    activeChord_.clear();
    activeChord_.addNote(chordGen_.getNoteNum(buttonId));
    activeButtonId_ = buttonId;
    sendNote(activeChord_[0]);
    if (ui_) {
      ui_->setChordRoot(toString(activeChord_[0] % 12));
    }
  }
}

void MidiController::processNoteOff(int root) {
  // ignore release of buttons that didn't start the current chord
  if (root != activeButtonId_) {
    return;
  }

  stopCurrentChord();

  if (ui_) {
    ui_->setChordRoot("-");
    ui_->setChordQuality("-");
  }
}

void MidiController::stopCurrentChord() {
  // send note off for all notes in the active chord
  for (size_t i = 0; i < activeChord_.size; i++) {
    output_.noteOff(activeChord_[i]);
  }
  activeChord_.clear();

  // send note off for last arp note if any
  if (lastArpNote_ != -1) {
    output_.noteOff(lastArpNote_);
    lastArpNote_ = -1;
  }

  // clear the active root
  activeButtonId_ = -1;

  sequencer_.clear();
}

void MidiController::setChordMode(bool enabled) { chordMode_ = enabled; }

void MidiController::setVelocity(float velocity) {
  defaultVelocity_ = velocity;
}

void MidiController::setUIController(UIController &ui) { ui_ = &ui; }

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
  if (sequencer_.isEnabled()) {
    sequencer_.setChord(chord);
  } else {
    for (size_t i = 0; i < chord.size; i++) {
      sendNote(chord[i]);
    }
  }
}

void MidiController::update() {
  if (sequencer_.isEnabled() && sequencer_.shouldPlayNow()) {
    // In arp mode: turn off the previous note before playing the next one
    if (sequencer_.isArpOn() && lastArpNote_ != -1) {
      output_.noteOff(lastArpNote_);
    }

    uint8_t nextNote = sequencer_.getNextNoteNum();
    sendNote(nextNote);

    // Track the note for arp mode
    if (sequencer_.isArpOn()) {
      lastArpNote_ = nextNote;
    }
  }
}

Sequencer MidiController::getSequencer() { return sequencer_; }
