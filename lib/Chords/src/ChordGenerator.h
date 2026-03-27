#pragma once

#include "Theory.h"

class ChordGenerator {
public:
  Scale scale;
  int octave;

  ChordGenerator(Scale scale = Scale{Notes::C}, int octave = 4);

  int getNoteNum(int root);
};
