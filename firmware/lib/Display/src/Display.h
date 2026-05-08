#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <lvgl.h>

class Display {
public:
  Display();
  void begin();
  void update();

  void switchTheme(int themeId);

private:
  static lv_display_t* _disp;

  static uint8_t* _drawBuf;

  static uint32_t getTick();
  void reset();

  static void sendCmd(lv_display_t* disp, const uint8_t* cmd, size_t cmd_size,
                      const uint8_t* param, size_t param_size);
  static void sendColor(lv_display_t* disp, const uint8_t* cmd, size_t cmd_size,
                        uint8_t* param, size_t param_size);
};
