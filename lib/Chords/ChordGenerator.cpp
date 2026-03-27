#include "ChordGenerator.h"

ChordGenerator::ChordGenerator(Scale scale, int octave)
    : scale(scale),
      octave(octave) {}

int ChordGenerator::getNoteNum(int root) {
  return octave * 12 + (int)scale.root + SCALE_INTERVALS[(int)scale.mode][root];
}
