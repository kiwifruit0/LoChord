#include "ChordGenerator.h"

ChordGenerator::ChordGenerator(Scale scale, int octave)
    : scale(scale),
      octave(octave) {}

int ChordGenerator::getNoteNum(int root) {
  return 24 + octave * 12 + static_cast<int>(scale.root) + SCALE_INTERVALS[static_cast<size_t>(scale.mode)][root];
}
