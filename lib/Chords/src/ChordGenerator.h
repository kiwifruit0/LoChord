#pragma once

#include "Theory.h"

class ChordGenerator {
public:
  Scale scale_;
  int octave_;

  ChordGenerator(Scale scale = Scale{Notes::C}, int octave = 3);

  // takes root and returns midi note num
  int getNoteNum(int root);

  // takes root and returns chord with proper midi values
  Chord getMidiChord(int root, int joystickPos);

  Quality joystickPosToQuality(int joystickPos);
};
