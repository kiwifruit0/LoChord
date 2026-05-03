#include "Button.h"
#include "Clock.h"
#include "Display.h"
#include "Joystick.h"
#include "MidiController.h"
#include "MidiOutput.h"
#include "RotaryEncoder.h"
#include "config.h"
#include <Arduino.h>
#include <USB.h>
#include <cstdlib>

Button buttons[NUM_BUTTONS];
RotaryEncoder encoders[NUM_MAPPABLE_ENCODERS];
Joystick joystick;
Clock mainClock(120);
MidiOutput midiOutput;
ChordGenerator chordGen;
MidiController midiController(chordGen, mainClock, midiOutput);
Display display;

void setup() {
  Serial.begin(115200);
  midiOutput.begin();
  USB.begin();
  std::srand(micros());

  for (int i = 0; i < NUM_BUTTONS; i++)
    buttons[i].begin(buttonPins[i]);
  for (int i = 0; i < NUM_MAPPABLE_ENCODERS; i++)
    encoders[i].begin(mappableEncoderPins[i]);
  joystick.begin(joystickPin);
  display.begin();

  delay(2000);
}

void loop() {
  joystick.update();
  display.update();

  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].update();
    if (buttons[i].wasPressed() || joystick.wasChanged()) {
      midiController.processNoteOn(i, joystick.getPos());
    } else if (buttons[i].wasReleased()) {
      midiController.processNoteOff(i);
    }
  }
  midiController.update();
}
