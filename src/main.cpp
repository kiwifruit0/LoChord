#include "Button.h"
#include "Clock.h"
#include "MidiController.h"
#include "MidiOutput.h"
#include "Strummer.h"
#include "config.h"
#include <Arduino.h>
#include <USB.h>

Button buttons[NUM_BUTTONS];

Clock mainClock(120);
MidiOutput midiOutput;
ChordGenerator chordGen;
MidiController midiController(chordGen, mainClock, midiOutput);
// Strummer strummer(mainClock);

void setup() {
  midiOutput.begin();
  USB.begin();

  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].begin(buttonPins[i]);
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
  // strummer.update();
}
