#include "ChordGenerator.h"

ChordGenerator::ChordGenerator(Scale scale, int octave)
    : scale_(scale), octave_(octave) {}

int ChordGenerator::getNoteNum(int root) {
  return 24 + octave_ * 12 + static_cast<int>(scale_.root) +
         getScaleIntervals(scale_.mode)[root];
}

ChordResult ChordGenerator::getMidiChord(int buttonId, int joystickPos) {
  Quality baseQuality = getQualityInScale(scale_.mode, buttonId);
  Quality extendedQuality = getExtendedQuality(baseQuality, joystickPos);
  Chord chord = getChordFromQuality(extendedQuality);
  int rootNote = getNoteNum(buttonId);
  for (size_t i = 0; i < chord.size; i++) {
    chord.notes[i] = chord[i] + rootNote;
  }
  return {chord, extendedQuality};
}
