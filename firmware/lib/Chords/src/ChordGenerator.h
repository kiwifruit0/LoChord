#pragma once

#include "Theory.h"

struct ChordResult {
  Chord chord;
  Quality quality;
  Quality baseQuality;
};

class ChordGenerator {
public:
  Scale scale_;
  int octave_;

  ChordGenerator(Scale scale = Scale{Notes::Cs, Mode::Major}, int octave = 3);

  // takes root and returns midi note num
  int getNoteNum(int root);

  // takes midi note number and returns Notes object
  Notes getNoteObj(int noteNum);

  // takes root and returns chord with proper midi values
  ChordResult getMidiChord(int root, int joystickPos);
};
