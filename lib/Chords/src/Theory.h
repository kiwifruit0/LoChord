#pragma once
#include <array>
#include <cstddef>

// Enum classes are excellent here - type-safe and can't be implicitly converted to int
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

// Type aliases make intent clearer
using ScaleIntervals = std::array<int, 7>;
using ScaleQualities = std::array<Qualities, 7>;

// Array is better than unordered_map for small, fixed-size data
// Indexed by Mode enum (0=Major, 1=Minor)
inline const std::array<ScaleIntervals, 2> SCALE_INTERVALS = {{
    {0, 2, 4, 5, 7, 9, 11}, // Major: W-W-H-W-W-W-H
    {0, 2, 3, 5, 7, 8, 10}  // Minor: W-H-W-W-H-W-W
}};

inline const std::array<ScaleQualities, 2> SCALE_QUALITIES = {{
    {
        Qualities::M, // I
        Qualities::m, // ii
        Qualities::m, // iii
        Qualities::M, // IV
        Qualities::M, // V
        Qualities::m, // vi
        Qualities::d, // vii°
    },
    {
        Qualities::m, // i
        Qualities::d, // ii°
        Qualities::M, // III
        Qualities::m, // iv
        Qualities::m, // v
        Qualities::M, // VI
        Qualities::M, // VII
    }
}};

// Helper functions provide consistent, safe access
inline const auto &getScaleIntervals(Mode mode) {
  return SCALE_INTERVALS[static_cast<size_t>(mode)];
}

inline const auto &getScaleQualities(Mode mode) {
  return SCALE_QUALITIES[static_cast<size_t>(mode)];
}
