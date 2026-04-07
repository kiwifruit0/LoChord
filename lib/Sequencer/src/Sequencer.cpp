#include "Sequencer.h"
#ifdef ARDUINO
#include "esp32-hal.h"
#endif
#include <ctime>
#include <stdexcept>

Sequencer::Sequencer(Clock &clock)
    : clock_(clock) {}

bool Sequencer::shouldPlayNow() {
  if (pos_ >= chord_.size || pos_ == -1) {
    return false;
  }
#ifdef ARDUINO
  uint32_t currTime = micros();
#else
  uint32_t currTime = clock() / (CLOCKS_PER_SEC / 1000000);
#endif
  // use appropriate denom based on active mode
  uint8_t denom = strumOn_ ? strumDenom_ : arpDenom_;
  // if current time is >= lastTime + 1 beat : send next note
  if (currTime >=
      lastTime_ + clock_.beatsToMicros(4 / static_cast<float>(denom))) {
#ifdef ARDUINO
    lastTime_ = micros();
#else
    lastTime_ = clock() / (CLOCKS_PER_SEC / 1000000);
#endif
    return true;
  };
  return false;
}

void Sequencer::setChord(const Chord &chord) {
  chord_ = chord;

  // set position based on mode
  if (strumOn_) {
    // strum mode
    if (strumMode_ == 'd') {
      pos_ = chord.size - 1;
    } else {
      pos_ = 0;
    }
    if (strumMode_ == 'r') {
      randomiseChordNotes();
    }
  } else if (arpOn_) {
    // arp mode
    switch (arpMode_) {
    case ArpMode::Down:
    case ArpMode::DownUp:
      pos_ = chord.size - 1;
      // start going down
      arpForward_ = false;
      break;
    case ArpMode::Converge:
      leftPos_ = 0;
      rightPos_ = chord.size - 1;
      useLeftPointer_ = true;
      pos_ = 0;
      break;
    case ArpMode::Diverge:
      // start from center
      leftPos_ = chord.size / 2;
      rightPos_ = (chord.size - 1) / 2;
      if (chord.size % 2 == 0) {
        rightPos_ = leftPos_ - 1;
      }
      useLeftPointer_ = true;
      pos_ = leftPos_;
      break;
    default:
      pos_ = 0;
      arpForward_ = true;
      break;
    }
  }
}

void Sequencer::setStrumOn(bool strumOn) {
  strumOn_ = strumOn;
  if (strumOn) {
    arpOn_ = 0;
  }
}

void Sequencer::setArpOn(bool arpOn) {
  arpOn_ = arpOn;
  if (arpOn) {
    strumOn_ = false;
    // reset arp state
    arpForward_ = true;
    leftPos_ = 0;
    rightPos_ = 0;
    useLeftPointer_ = true;
  }
}

bool Sequencer::isEnabled() { return strumOn_ || arpOn_; }

bool Sequencer::isArpOn() { return arpOn_; }

void Sequencer::clear() { pos_ = -1; }

void Sequencer::setStrumDenom(uint8_t amount) { strumDenom_ = amount; }
uint8_t Sequencer::getStrumDenom() { return strumDenom_; }

uint8_t Sequencer::getNextNoteNum() {
  if (strumOn_) {
    if (strumMode_ == 'd') {
      return chord_[pos_--];
    } else {
      // both up and random iterate through chord in order
      return chord_[pos_++];
    }
  } else if (arpOn_) {
    // looping playback while held down
    uint8_t note;

    switch (arpMode_) {
    case ArpMode::Up:
      note = chord_[pos_];
      // wrap around
      pos_ = (pos_ + 1) % chord_.size;
      break;

    case ArpMode::Down:
      note = chord_[pos_];
      if (pos_ == 0) {
        // wrap to end
        pos_ = chord_.size - 1;
      } else {
        pos_--;
      }
      break;

    case ArpMode::UpDown:
      note = chord_[pos_];
      if (arpForward_) {
        // going up
        if (pos_ >= chord_.size - 1) {
          // change direction
          arpForward_ = false;
          if (chord_.size > 1) {
            // dont repeat top note
            pos_--;
          }
        } else {
          pos_++;
        }
      } else {
        // going down
        if (pos_ == 0) {
          // change direction
          arpForward_ = true;
          if (chord_.size > 1) {
            // dont repeat bottom note
            pos_++;
          }
        } else {
          pos_--;
        }
      }
      break;

    case ArpMode::DownUp:
      note = chord_[pos_];
      if (!arpForward_) {
        // going down
        if (pos_ == 0) {
          // change direction
          arpForward_ = true;
          if (chord_.size > 1) {
            // don't repeat bottom note
            pos_++;
          }
        } else {
          pos_--;
        }
      } else {
        // going up
        if (pos_ >= chord_.size - 1) {
          // change direction
          arpForward_ = false;
          if (chord_.size > 1) {
            // don't repeat top note
            pos_--;
          }
        } else {
          pos_++;
        }
      }
      break;

    case ArpMode::Converge:
      // alternate between outer notes moving in
      if (useLeftPointer_) {
        note = chord_[leftPos_];
        leftPos_++;
      } else {
        note = chord_[rightPos_];
        if (rightPos_ > 0) {
          rightPos_--;
        }
      }
      useLeftPointer_ = !useLeftPointer_;

      // reset when pointers meet or cross
      if (leftPos_ > rightPos_ || leftPos_ >= chord_.size) {
        leftPos_ = 0;
        rightPos_ = chord_.size - 1;
      }
      break;

    case ArpMode::Diverge:
      // start from center and move out
      if (useLeftPointer_) {
        note = chord_[leftPos_];
        if (leftPos_ > 0) {
          leftPos_--;
        }
      } else {
        note = chord_[rightPos_];
        rightPos_++;
      }
      useLeftPointer_ = !useLeftPointer_;

      // reset when covered full range
      if (leftPos_ == 0 && rightPos_ >= chord_.size) {
        leftPos_ = chord_.size / 2;
        rightPos_ = (chord_.size - 1) / 2;
        if (chord_.size % 2 == 0) {
          rightPos_ = leftPos_ - 1;
        }
      }
      break;

    case ArpMode::Random:
      // pick a random note from chord
      pos_ = rand() % chord_.size;
      note = chord_[pos_];
      break;
    }

    return note;
  }

  // fallback
  return chord_[0];
}

void Sequencer::setStrumMode(char mode) {
  if (mode != 'u' && mode != 'd' && mode != 'r') {
    throw std::runtime_error("StrumMode must be 'u', 'd' or 'r'");
  }
  strumMode_ = mode;
}

char Sequencer::getStrumMode() { return strumMode_; }

// loops through notes and swaps each with a random index
void Sequencer::randomiseChordNotes() {
  for (uint8_t i = 0; i < chord_.size; i++) {
    int currentVal = chord_[i];
    int newPos = rand() % chord_.size;
    chord_.notes[i] = chord_[newPos];
    chord_.notes[newPos] = currentVal;
  }
}

void Sequencer::setArpMode(ArpMode mode) {
  arpMode_ = mode;
  // reset direction for bidirectional modes
  arpForward_ = true;
}

ArpMode Sequencer::getArpMode() { return arpMode_; }

void Sequencer::setArpDenom(uint8_t denom) { arpDenom_ = denom; }

uint8_t Sequencer::getArpDenom() { return arpDenom_; }
