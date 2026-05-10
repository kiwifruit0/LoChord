#pragma once

#include <cstdint>

// buttons
#define NUM_BUTTONS 7

const uint8_t buttonPins[NUM_BUTTONS] = {36, 37, 38, 39, 40, 41, 42};

// encoders
#define PIN_ENCODER_A 48
#define PIN_ENCODER_B 45
#define PIN_ENCODER_BUTTON 47

// joystick
#define PIN_JOYSTICK_LR 0
#define PIN_JOYSTICK_UD 0
#define PIN_JOYSTICK_BUTTON 0

// display
#define PIN_LCD_DC 9
#define PIN_LCD_CS 10
#define PIN_LCD_RST 8
#define PIN_LCD_SCK 12
#define PIN_LCD_MOSI 11
#define PIN_LCD_MISO -1
#define PIN_LCD_BL 17
#define LCD_SPI_HZ 40000000
