#pragma once
#include <array>
#include <cstddef>
#include <vector>

enum class Notes { C, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B };

enum class Mode { Major, Minor };

enum class Qualities { M, m, d };

struct Note {
  Notes note;
  int octave;
};

struct Scale {
  Notes root;
  Mode mode;
};

using ScaleIntervals = std::array<int, 7>;

inline const std::array<ScaleIntervals, 2> SCALE_INTERVALS = {{
    {0, 2, 4, 5, 7, 9, 11}, // major
    {0, 2, 3, 5, 7, 8, 10}  // minor
}};

using ChordQualities = std::array<Qualities, 7>;

inline const std::array<ChordQualities, 2> SCALE_QUALITIES = {
    // major scale
    {{
         Qualities::M, // I
         Qualities::m, // ii
         Qualities::m, // iii
         Qualities::M, // IV
         Qualities::M, // V
         Qualities::m, // vi
         Qualities::d, // vii
     },
    // minor scale
     {
         Qualities::m, // I
         Qualities::d, // ii
         Qualities::M, // iii
         Qualities::m, // IV
         Qualities::m, // V
         Qualities::M, // vi
         Qualities::M, // vii

     }}};

inline const auto &getScaleIntervals(Mode mode) {
  return SCALE_INTERVALS[static_cast<size_t>(mode)];
}
