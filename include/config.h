#pragma once

#include <cstdint>

// buttons
#define NUM_BUTTONS 7

const uint8_t buttonPins[NUM_BUTTONS] = {42, 41, 40, 39, 38, 37, 36};

// encoders
const int uiEncoderPin = 23;

#define NUM_MAPPABLE_ENCODERS 2

const uint8_t mappableEncoderPins[NUM_MAPPABLE_ENCODERS] = {1, 2};

// joystick
const uint8_t joystickPin = 3;
