#pragma once
#include "ChordGenerator.h"
#include "Clock.h"
#include "MidiOutput.h"

class MidiController {
public:
  MidiController(ChordGenerator chordGen, Clock &clock, MidiOutput &output,
                 bool chordMode = true, float defaultVelocity = 100,
                 float strumAmt = 0, float randVelocityAmt = 0);

  void processNoteOn(int root);
  void processNoteOff(int root);

  void setChordMode(bool enabled);
  void setStrumAmount(float amount);
  void setVelocity(float velocity);

  ChordGenerator &getChordGenerator();

private:
  MidiOutput &output_;
  Clock &clock_;
  ChordGenerator chordGen_;
  bool chordMode_;
  float strumAmt_;
  float randVelocityAmt_;
  float defaultVelocity_;

  // tracks held notes for note off
  Chord activeChord_;

  void sendChord(const Chord &chord);
  void sendNote(int noteNum);
  uint8_t calculateVelocity();
};
