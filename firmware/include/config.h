#pragma once

#include <cstdint>

// See ../PINOUT.md — it is the source of truth. If this file and PINOUT.md
// disagree, this file is wrong.
//
// Target module: ESP32-S3-WROOM-1 N8R2 (quad PSRAM). Chord buttons 1-3 sit on
// GPIO35/36/37, which an octal-PSRAM module (N8R8/N16R8) uses internally.

// buttons
#define NUM_BUTTONS 7

// index order matches physical position: bottom row left->right, then top row
const uint8_t buttonPins[NUM_BUTTONS] = {35, 36, 37, 38, 39, 40, 41};

#define PIN_BUTTON_LOOP 46

// encoders
#define PIN_ENCODER_UI_A 21
#define PIN_ENCODER_UI_B 47
#define PIN_ENCODER_UI_SW 48

#define PIN_ENCODER_CTRL1_A 7
#define PIN_ENCODER_CTRL1_B 15
#define PIN_ENCODER_CTRL1_SW 16

#define PIN_ENCODER_CTRL2_A 17
#define PIN_ENCODER_CTRL2_B 18
#define PIN_ENCODER_CTRL2_SW 8

// joystick — both axes on ADC1, which stays usable with the radio enabled
#define PIN_JOYSTICK_LR 4  // ADC1_CH3
#define PIN_JOYSTICK_UD 5  // ADC1_CH4
#define PIN_JOYSTICK_BUTTON 6

// display — CS/MOSI/SCK are SPI2's IO_MUX pins, not routed via the GPIO matrix
#define PIN_LCD_BL 9
#define PIN_LCD_CS 10
#define PIN_LCD_MOSI 11
#define PIN_LCD_SCK 12
#define PIN_LCD_DC 13
#define PIN_LCD_RST 14
#define PIN_LCD_MISO -1
#define LCD_SPI_HZ 40000000

// audio — reserved for the future I2S DAC, not wired yet. See ../PINOUT.md
#define PIN_I2S_DOUT 42
#define PIN_I2S_BCLK 2
#define PIN_I2S_WS 1
