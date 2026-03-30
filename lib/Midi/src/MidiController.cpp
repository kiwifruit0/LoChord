#include "MidiController.h"
#include <Arduino.h>
#include <cstdlib>

MidiController::MidiController(ChordGenerator chordGen, Clock &clock,
                               MidiOutput &output, bool chordMode,
                               float defaultVelocity, float strumAmt,
                               float randVelocityAmt)
    : _output(output), _clock(clock), _chordGen(chordGen),
      _chordMode(chordMode), _strumAmt(strumAmt),
      _randVelocityAmt(randVelocityAmt), _defaultVelocity(defaultVelocity) {}

void MidiController::processNoteOn(int root) {
  if (_chordMode) {
    _activeChord = _chordGen.getMidiChord(root);
    sendChord(_activeChord);
  } else {
    _activeChord.clear();
    _activeChord.addNote(_chordGen.getNoteNum(root));
    sendNote(_activeChord[0]);
  }
}

void MidiController::processNoteOff(int root) {
  // Send note-off for all notes in the active chord
  for (size_t i = 0; i < _activeChord.size; i++) {
    _output.noteOff(_activeChord[i]);
  }
  _activeChord.clear();
}

void MidiController::setChordMode(bool enabled) { _chordMode = enabled; }

void MidiController::setStrumAmount(float amount) { _strumAmt = amount; }

void MidiController::setVelocity(float velocity) {
  _defaultVelocity = velocity;
}
ChordGenerator &MidiController::getChordGenerator() { return _chordGen; }

uint8_t MidiController::calculateVelocity() {
  if (_randVelocityAmt <= 0) {
    return static_cast<uint8_t>(_defaultVelocity);
  }
  int maxRandom = static_cast<int>(_defaultVelocity * _randVelocityAmt);
  int randomOffset = (maxRandom > 0) ? (rand() % maxRandom) : 0;
  return static_cast<uint8_t>(_defaultVelocity - randomOffset);
}

void MidiController::sendNote(int noteNum) {
  _output.noteOn(noteNum, calculateVelocity());
}

void MidiController::sendChord(const Chord &chord) {
  // TODO: Replace with Scheduler-based strum for non-blocking playback
  // Currently plays all notes simultaneously (strum disabled)
  for (size_t i = 0; i < chord.size; i++) {
    sendNote(chord.notes[i]);
  }
}
