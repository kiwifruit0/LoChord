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

enum class Qualities { Maj, min, dim, Aug, Maj7, min7 };

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

  Chord()
      : size(0) {};

  Chord(std::initializer_list<int> n)
      : size(n.size()) {
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
using ScaleQualities = std::array<Qualities, 7>;

inline const std::array<ScaleIntervals, 2> SCALE_INTERVALS = {{
    {0, 2, 4, 5, 7, 9, 11}, // major
    {0, 2, 3, 5, 7, 8, 10}  // minor
}};

inline const std::array<ScaleQualities, 2> SCALE_QUALITIES = {
    {// major
     {
         Qualities::Maj, // I
         Qualities::min, // ii
         Qualities::min, // iii
         Qualities::Maj, // IV
         Qualities::Maj, // V
         Qualities::min, // vi
         Qualities::dim, // vii*
     },
     // minor
     {
         Qualities::min, // i
         Qualities::dim, // ii*
         Qualities::Maj, // III
         Qualities::min, // iv
         Qualities::min, // v
         Qualities::Maj, // VI
         Qualities::Maj, // VII
     }}};

// triads
inline const std::array<Chord, 6> CHORD_INTERVALS = {
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
};

// ------------------------------
// helpers
// ------------------------------
inline const auto &getScaleIntervals(Mode mode) {
  return SCALE_INTERVALS[static_cast<size_t>(mode)];
}

inline const auto &getScaleQualities(Mode mode) {
  return SCALE_QUALITIES[static_cast<size_t>(mode)];
}
