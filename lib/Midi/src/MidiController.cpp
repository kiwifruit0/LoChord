#include "MidiController.h"
#include <Arduino.h>
#include <cstdlib>

MidiController::MidiController(ChordGenerator chordGen, int bpm, bool chordMode,
                               float defaultVelocity, float strumAmt,
                               float randVelocityAmt)
    : chordGen(chordGen),
      bpm(bpm),
      chordMode(chordMode),
      defaultVelocity(defaultVelocity),
      strumAmt(strumAmt),
      randVelocityAmt(randVelocityAmt) {};

void MidiController::processNoteOn(int root) {
  if (this->chordMode) {
    sendChord(this->chordGen.getMidiChord(root));
  } else {
    sendNote(this->chordGen.getNoteNum(root));
  }
}

void MidiController::sendNote(int noteNum) {
  // gives the max random velocity that can be applied to the note
  // v - v * (1 - randVelocityAmt)
  int maxRandomVelocity = this->defaultVelocity -
                          (this->defaultVelocity * (1 - this->randVelocityAmt));
  int randomVelocity = rand() % maxRandomVelocity;

  // send velocity = default - random
  this->midi.noteOn(noteNum, this->defaultVelocity - randomVelocity);
}

void MidiController::sendChord(Chord chord) {
  // maximum ms between each note is 1 beat
  int strumMs = 60 * (1000 / this->bpm);
  for (int i = 0; i < chord.size; i++) {
    sendNote(chord.notes[i]);
    delay(strumMs);
  }
}
