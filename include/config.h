#pragma once

#include <cstdint>

// buttons
#define NUM_BUTTONS 2

const uint8_t buttonPins[NUM_BUTTONS] = {45, 35};

// encoders
const int UI_ENCODER_PIN = 23;

#define NUM_MAPPABLE_ENCODERS 2

const uint8_t mappableEncoderPins[NUM_MAPPABLE_ENCODERS] = {1, 2};
