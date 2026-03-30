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
  MidiOutput &_output;
  Clock &_clock;
  ChordGenerator _chordGen;
  bool _chordMode;
  float _strumAmt;
  float _randVelocityAmt;
  float _defaultVelocity;

  // Tracks currently held notes for proper note-off
  Chord _activeChord;

  void sendChord(const Chord &chord);
  void sendNote(int noteNum);
  uint8_t calculateVelocity();
};
