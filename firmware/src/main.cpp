#include "Button.h"
#include "Clock.h"
#include "Display.h"
#include "Joystick.h"
#include "MidiController.h"
#include "MidiOutput.h"
#include "RotaryEncoder.h"
#include "UIController.h"
#include "config.h"
#include <Arduino.h>
#include <USB.h>
#include <cstdlib>

Button buttons[NUM_BUTTONS];
RotaryEncoder encoder;
Joystick joystick;
Clock mainClock(120);
MidiOutput midiOutput;
ChordGenerator chordGen;
MidiController midiController(chordGen, mainClock, midiOutput);
Display display;
UIController ui(display);

void setup() {
  Serial.begin(115200);
  midiOutput.begin();
  USB.begin();
  std::srand(micros());

  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].begin(buttonPins[i]);
  };

  encoder.begin(PIN_ENCODER_A, PIN_ENCODER_B, PIN_ENCODER_BUTTON);

  joystick.begin(PIN_JOYSTICK_UD, PIN_JOYSTICK_LR, PIN_JOYSTICK_BUTTON);
  display.begin();

  midiController.setUIController(ui);

  delay(2000);
}

void loop() {
  display.update();
  encoder.update();
  joystick.update();

  ui.updateUsbStatus();

  if (encoder.getButton().wasPressed()) {
    Serial.println("encoder button pressed");
  }

  int delta = encoder.getDelta();
  if (delta != 0) {
    Serial.println("encoder turned " + String(delta));
  }

  if (joystick.getButton().wasPressed()) {
    Serial.println("joystick button pressed");
  }

  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].update();
    if (buttons[i].wasPressed() || joystick.wasChanged()) {
      midiController.processNoteOn(i, 4);
      Serial.println("button " + String(i) + " pressed");
    } else if (buttons[i].wasReleased()) {
      midiController.processNoteOff(i);
    }
  }
  midiController.update();
}
