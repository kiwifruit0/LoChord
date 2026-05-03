#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <lvgl.h>

class Display {
public:
  Display();
  void begin();
  void update(); // call in loop()

private:
  static void sendCmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size,
                      const uint8_t *param, size_t param_size);

  static void sendColor(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size,
                        uint8_t *param, size_t param_size);

  static void reset();
  static uint32_t getTick();

  lv_display_t *_disp = nullptr;
  static uint8_t _drawBuf[142 * 40 * 2];
};
