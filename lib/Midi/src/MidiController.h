#pragma once
#include "ChordGenerator.h"
#include "Clock.h"
#include "MidiOutput.h"
#include "Sequencer.h"

class MidiController {
public:
  MidiController(ChordGenerator chordGen, Clock &clock, MidiOutput &output,
                 bool chordMode = true, bool strumOn = true,
                 bool arpOn = false, float defaultVelocity = 100,
                 float randVelocityAmt = 0);

  void processNoteOn(int root, int joystickPos);
  void processNoteOff(int root);

  void setChordMode(bool enabled);
  void setVelocity(float velocity);
  void setStrumOn(bool strumOn);
  void setArpOn(bool arpOn);

  ChordGenerator &getChordGenerator();

  void update();

  Sequencer getSequencer();

private:
  // objects
  MidiOutput &output_;
  Clock &clock_;
  ChordGenerator chordGen_;
  Sequencer sequencer_;

  // attr
  bool chordMode_;
  float randVelocityAmt_;
  float defaultVelocity_;

  // tracks held notes for note off
  Chord activeChord_;
  
  // tracks which root/button started the current chord
  int activeRoot_ = -1;
  
  // tracks the last arp note playing (for note-off in arp mode)
  int lastArpNote_ = -1;

  void sendNote(int noteNum);
  void sendChord(const Chord &chord);
  void stopCurrentChord();
  uint8_t calculateVelocity();
};
