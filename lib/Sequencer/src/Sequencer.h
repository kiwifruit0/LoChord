#pragma once
#include "Clock.h"
#include "Theory.h"
#include <cstdint>

enum class ArpMode { Up, Down, UpDown, DownUp, Converge, Diverge, Random };

class Sequencer {
public:
  Sequencer(Clock &clock);

  bool shouldPlayNow();

  void setChord(const Chord &chord);

  void clear();

  void setStrumOn(bool strumOn);
  void setArpOn(bool arpOn);

  bool isEnabled();

  void setArpMode(ArpMode mode);
  ArpMode getArpMode();

  void setArpDenom(uint8_t denom);
  uint8_t getArpDenom();

  void setStrumDenom(uint8_t denom);
  uint8_t getStrumDenom();

  uint8_t getNextNoteNum();

  void setStrumMode(char mode);

  char getStrumMode();

private:
  Chord chord_;
  size_t pos_;
  bool strumOn_ = false;
  bool arpOn_ = true;

  // strum timing - e.g. strumDenom = 4: every note played 1/4ths apart
  uint8_t strumDenom_ = 8;

  // arp timing - e.g. arpDenom = 16: every note played 1/16ths apart
  uint8_t arpDenom_ = 16;

  // arp mode
  ArpMode arpMode_ = ArpMode::Up;

  // direction tracking for bidirectional arp modes (true = forward, false =
  // backward)
  bool arpForward_ = true;

  // for converge/diverge modes - track left and right pointers
  size_t leftPos_ = 0;
  size_t rightPos_ = 0;
  // alternate between left and right
  bool useLeftPointer_ = true;

  uint32_t lastTime_;
  Clock &clock_;

  // u: up, d: down, r: random
  char strumMode_ = 'u';

  void randomiseChordNotes();
};
