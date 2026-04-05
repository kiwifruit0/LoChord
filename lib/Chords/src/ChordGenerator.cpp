#include "ChordGenerator.h"

ChordGenerator::ChordGenerator(Scale scale, int octave)
    : scale_(scale), octave_(octave) {}

int ChordGenerator::getNoteNum(int root) {
  return 24 + octave_ * 12 + static_cast<int>(scale_.root) +
         getScaleIntervals(scale_.mode)[root];
}

Chord ChordGenerator::getMidiChord(int root, int joystickPos) {
  Quality baseQuality = getQualityInScale(scale_.mode, root);
  Quality extendedQuality = getExtendedQuality(baseQuality, joystickPos);
  Chord chord = getChordFromQuality(extendedQuality);
  int rootNote = getNoteNum(root);
  for (size_t i = 0; i < chord.size; i++) {
    chord.notes[i] = chord[i] + rootNote;
  }
  return chord;
}
