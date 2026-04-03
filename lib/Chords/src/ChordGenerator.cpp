#include "ChordGenerator.h"

ChordGenerator::ChordGenerator(Scale scale, int octave)
    : scale(scale), octave(octave) {}

int ChordGenerator::getNoteNum(int root) {
  return 24 + octave * 12 + static_cast<int>(scale.root) +
         getScaleIntervals(scale.mode)[root];
}

Chord ChordGenerator::getMidiChord(int root) {

  Quality quality = getScaleQualities(scale.mode)[root];
  Chord chord = getChordFromQuality(quality);
  int rootNote = getNoteNum(root);
  for (size_t i = 0; i < chord.size; i++) {
    chord.notes[i] = chord[i] + rootNote;
  }
  return chord;
}
