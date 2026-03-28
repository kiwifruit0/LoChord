#include "MidiController.h"

MidiController::MidiController(bool chordMode, float defaultVelocity,
                               float strumAmt, float randVelocityAmt)
    : chordMode(chordMode),
      defaultVelocity(defaultVelocity),
      strumAmt(strumAmt),
      randVelocityAmt(randVelocityAmt) {};

USBMIDI MidiController::getMidiDevice() { return this->midi; }
