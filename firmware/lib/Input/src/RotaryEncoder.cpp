#include "RotaryEncoder.h"
#include <Arduino.h>

RotaryEncoder *RotaryEncoder::instance_ = nullptr;

void RotaryEncoder::begin(uint8_t pinA, uint8_t pinB, uint8_t buttonPin) {
  pinA_ = pinA;
  pinB_ = pinB;

  button_.begin(buttonPin);

  instance_ = this;

  pinMode(pinA_, INPUT_PULLUP);
  pinMode(pinB_, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(pinA_), isrTrampoline, CHANGE);
}

void RotaryEncoder::isrTrampoline() {
  if (instance_) {
    instance_->handleIsr();
  }
}

void RotaryEncoder::handleIsr() {
  counter_ += (digitalRead(pinB_) != digitalRead(pinA_)) ? -1 : 1;
}

void RotaryEncoder::update() { button_.update(); }

int RotaryEncoder::getDelta() {
  int delta = counter_ - lastCounter_;
  lastCounter_ = counter_;
  return delta;
}
