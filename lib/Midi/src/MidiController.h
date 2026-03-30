#pragma once
#include "ChordGenerator.h"
#include <USBMIDI.h>

class MidiController {
public:
  MidiController(ChordGenerator chordGen, int bpm = 120, bool chordMode = true,
                 float defaultVelocity = 100, float strumAmt = 0,
                 float randVelocityAmt = 0);

  void processNoteOn(int root);

private:
  USBMIDI midi;
  ChordGenerator chordGen;
  int bpm;
  bool chordMode;
  float strumAmt;
  float randVelocityAmt;
  float defaultVelocity;

  void sendChord(Chord chord);
  void sendNote(int noteNum);
};
