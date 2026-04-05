#pragma once
#include <array>
#include <cstddef>

/**
 * order for scales:
 * 0: major
 * 1: minor
 *
 * order for qualities:
 * 0: maj
 * 1: min
 * 2: dim
 * 3: aug
 * 4: maj7
 * 5: min7
 */

enum class Notes { C, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B };

enum class Mode { Major, Minor };

enum class Quality {
  Maj,
  min,
  dim,
  Aug,
  Maj7,
  min7,
  Maj9,
  min9,
  sus2,
  sus4,
  dom7,
  dim7,
  dim9
};

// ------------------------------
// structs
// ------------------------------
struct Note {
  Notes note;
  int octave;
};

struct Scale {
  Notes root;
  Mode mode;
};

constexpr int MAX_CHORD_NOTES = 7;

struct Chord {
  std::array<int, MAX_CHORD_NOTES> notes;
  std::size_t size;

  Chord() : size(0) {};

  Chord(std::initializer_list<int> n) : size(n.size()) {
    size_t i = 0;
    for (int note : n) {
      if (i < MAX_CHORD_NOTES) {
        notes[i++] = note;
      }
    }
  }

  void addNote(int midiNote) {
    if (size < MAX_CHORD_NOTES) {
      notes[size++] = midiNote;
    }
  }

  void clear() { size = 0; }

  int operator[](size_t i) const { return notes[i]; }

  const int *begin() const { return notes.data(); }
  const int *end() const { return notes.data() + size; }
};

// ------------------------------
// theory data
// ------------------------------

using ScaleIntervals = std::array<int, 7>;
using ScaleQualities = std::array<Quality, 7>;
using JoystickChords = std::array<Quality, 9>;

inline const std::array<ScaleIntervals, 2> SCALE_INTERVALS = {{
    {0, 2, 4, 5, 7, 9, 11}, // major
    {0, 2, 3, 5, 7, 8, 10}  // minor
}};

// maps scales to the types of chords in them
inline const std::array<ScaleQualities, 2> SCALE_QUALITIES = {
    {// major
     {
         Quality::Maj, // I
         Quality::min, // ii
         Quality::min, // iii
         Quality::Maj, // IV
         Quality::Maj, // V
         Quality::min, // vi
         Quality::dim, // vii*
     },
     // minor
     {
         Quality::min, // i
         Quality::dim, // ii*
         Quality::Maj, // III
         Quality::min, // iv
         Quality::min, // v
         Quality::Maj, // VI
         Quality::Maj, // VII
     }}};

// maps chords to their extended possibilities with the joystick
// centre first, then starts at top of joystick and goes clockwise
inline const std::array<JoystickChords, 3> EXTENDED_CHORDS = {{
    // major chord extensions
    {
        Quality::Maj,  // centre
        Quality::min,  // up
        Quality::dom7, // up right
        Quality::Maj7, // right
        Quality::Maj9, // down right
        Quality::sus4, // down
        Quality::sus2, // down left
        Quality::dim,  // left
        Quality::Aug   // up left
    },
    //  minor extensions
    {
        Quality::min,  // centre
        Quality::Maj,  // up
        Quality::dom7, // up right
        Quality::min7, // right
        Quality::min9, // down right
        Quality::sus4, // down
        Quality::sus2, // down left
        Quality::dim,  // left
        Quality::Aug   // up left
    },
    //  diminished extensions
    {
        Quality::dim,  // centre
        Quality::Maj,  // up
        Quality::dom7, // up right
        Quality::dim7, // right
        Quality::dim9, // down right
        Quality::sus4, // down
        Quality::sus2, // down left
        Quality::min,  // left
        Quality::Aug   // up left
    },
}};

// chords initialised with intervals
inline const std::array<Chord, 13> CHORDS = {
    // Maj
    Chord({0, 4, 7}),
    // min
    Chord({0, 3, 7}),
    // dim
    Chord({0, 3, 6}),
    // Aug
    Chord({0, 4, 8}),
    // Maj7
    Chord({0, 4, 7, 11}),
    // min7
    Chord({0, 3, 7, 10}),
    // Maj9
    Chord({0, 4, 7, 11, 14}),
    // min9
    Chord({0, 3, 7, 10, 14}),
    // sus2
    Chord({0, 2, 6}),
    // sus4
    Chord({0, 4, 6}),
    // dom7
    Chord({0, 4, 8}),
    // dim7
    Chord({0, 3, 6, 10}),
    // dim9
    Chord({0, 3, 6, 10, 13})};

// ------------------------------
// helpers
// ------------------------------
inline const auto &getScaleIntervals(Mode mode) {
  return SCALE_INTERVALS[static_cast<size_t>(mode)];
}


inline const auto &getQualityInScale(Mode scaleMode, int root) {
  return SCALE_QUALITIES[static_cast<size_t>(scaleMode)][root];
}

// takes quality and returns chord with intervals and root 0
inline const auto &getChordFromQuality(Quality quality) {
  return CHORDS[static_cast<size_t>(quality)];
}

inline const auto &getExtendedQuality(Quality baseQuality, int joystickPos) {
  return EXTENDED_CHORDS[static_cast<size_t>(baseQuality)][joystickPos];
}
