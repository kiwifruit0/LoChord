#include "Button.h"
#include "Clock.h"
#include "MidiController.h"
#include "MidiOutput.h"
#include "RotaryEncoder.h"
#include "config.h"
#include <Arduino.h>
#include <USB.h>
#include <cstdlib>

// input hardware
Button buttons[NUM_BUTTONS];
RotaryEncoder encoders[NUM_MAPPABLE_ENCODERS];

Clock mainClock(120);
MidiOutput midiOutput;
ChordGenerator chordGen;
MidiController midiController(chordGen, mainClock, midiOutput);

void setup() {
  midiOutput.begin();
  USB.begin();

  std::srand(micros());

  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].begin(buttonPins[i]);
  }

  for (int i = 0; i < NUM_MAPPABLE_ENCODERS; i++) {
    encoders[i].begin(mappableEncoderPins[i]);
  }

  delay(2000);
}

void loop() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].update();

    if (buttons[i].wasPressed()) {
      midiController.processNoteOn(i);
    } else if (buttons[i].wasReleased()) {
      midiController.processNoteOff(i);
    }
  }
  midiController.update();
}
