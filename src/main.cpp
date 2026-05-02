#include "Button.h"
#include "Clock.h"
#include "Joystick.h"
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
Joystick joystick;

Clock mainClock(120);
MidiOutput midiOutput;
ChordGenerator chordGen;
MidiController midiController(chordGen, mainClock, midiOutput);

void setup() {
  Serial.begin(115200);
  midiOutput.begin();
  USB.begin();

  std::srand(micros());

  // // hardware setup
  // for (int i = 0; i < NUM_BUTTONS; i++) {
  //   buttons[i].begin(buttonPins[i]);
  // }
  // for (int i = 0; i < NUM_MAPPABLE_ENCODERS; i++) {
  //   encoders[i].begin(mappableEncoderPins[i]);
  // }
  // joystick.begin(joystickPin);

  // delay for detection as a midi device
  delay(2000);
}

void loop() {
  // joystick.update();
  // for (int i = 0; i < NUM_BUTTONS; i++) {
  //   buttons[i].update();
  //
  //   if (buttons[i].wasPressed() || joystick.wasChanged()) {
  //     midiController.processNoteOn(i, joystick.getPos());
  //     Serial.println("Button " + String(i) + " pressed. Joystick pos: " +
  //     String(joystick.getPos()));
  //   } else if (buttons[i].wasReleased()) {
  //     midiController.processNoteOff(i);
  //   }
  // }
  midiController.update();

  Serial.print("MOSI: ");
  Serial.println(MOSI);
  Serial.print("MISO: ");
  Serial.println(MISO);
  Serial.print("SCK: ");
  Serial.println(SCK);
  Serial.print("SS: ");
  Serial.println(SS);

  delay(1000);
}
