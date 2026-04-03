#pragma once

#include "Theory.h"

class ChordGenerator {
public:
  Scale scale;
  int octave;

  ChordGenerator(Scale scale = Scale{Notes::C}, int octave = 3);

  int getNoteNum(int root);

  // takes root and returns chord with proper midi values
  Chord getMidiChord(int root);
};
