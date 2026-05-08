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

constexpr const char *NOTE_NAMES[] = {"C",  "C#", "D",  "D#", "E",  "F",
                                      "F#", "G",  "G#", "A",  "A#", "B"};

constexpr const char *QUALITY_NAMES[] = {"Maj",  "min",  "dim",  "Aug",  "Maj7",
                                         "min7", "Maj9", "min9", "sus2", "sus4",
                                         "dom7", "dim7", "dim9"};

constexpr const char *MODE_NAMES[] = {"Major", "Minor"};

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
        Quality::Aug,   // up left
        Quality::min,  // up
        Quality::dom7, // up right
        Quality::dim,  // left
        Quality::Maj,  // centre
        Quality::Maj7, // right
        Quality::sus2, // down left
        Quality::sus4, // down
        Quality::Maj9, // down right
    },
    //  minor extensions
    {
        Quality::Aug,  // up left
        Quality::Maj,  // up
        Quality::dom7, // up right
        Quality::dim,  // left
        Quality::min,  // centre
        Quality::min7, // right
        Quality::sus2, // down left
        Quality::sus4, // down
        Quality::min9, // down right
    },
    //  diminished extensions
    {
        Quality::Aug,  // up left
        Quality::Maj,  // up
        Quality::dom7, // up right
        Quality::min,  // left
        Quality::dim,  // centre
        Quality::dim7, // right
        Quality::sus2, // down left
        Quality::sus4, // down
        Quality::dim9, // down right
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
    Chord({0, 2, 7}),
    // sus4
    Chord({0, 5, 7}),
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

inline const char *toString(int midiNum) {
  return NOTE_NAMES[static_cast<size_t>(midiNum % 12)];
}

inline const char *toString(Quality q) {
  return QUALITY_NAMES[static_cast<size_t>(q)];
}

inline const char *toString(Mode m) {
  return MODE_NAMES[static_cast<size_t>(m)];
}

inline const char *toString(const Chord &chord) {
  static char buf[32];
  char *p = buf;
  for (int i = 0; i < chord.size; ++i) {
    if (i > 0)
      *p++ = ' ';
    const char *name = toString(chord[i]);
    while (*name)
      *p++ = *name++;
  }
  *p = '\0';
  return buf;
}
