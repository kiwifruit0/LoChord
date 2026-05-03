#include "Display.h"
#include "ui.h"
#include "config.h"

#define LCD_HOR_RES 142
#define LCD_VER_RES 428

// ── Static members ────────────────────────────────────
uint8_t Display::_drawBuf[LCD_HOR_RES * 40 * 2];

// ── Constructor ───────────────────────────────────────
Display::Display() {}

// ── Public ────────────────────────────────────────────
void Display::begin() {
  // Backlight + GPIO
  pinMode(PIN_LCD_BL, OUTPUT);
  digitalWrite(PIN_LCD_BL, HIGH);
  pinMode(PIN_LCD_DC, OUTPUT);
  pinMode(PIN_LCD_CS, OUTPUT);
  digitalWrite(PIN_LCD_CS, HIGH);
  pinMode(PIN_LCD_RST, OUTPUT);

  reset();

  SPI.begin(PIN_LCD_SCK, PIN_LCD_MISO, PIN_LCD_MOSI, PIN_LCD_CS);
  delay(100);

  // LVGL
  lv_init();
  lv_tick_set_cb(getTick);

  _disp = lv_nv3007_create(LCD_HOR_RES, LCD_VER_RES, LV_LCD_FLAG_NONE, sendCmd,
                           sendColor);

  lv_nv3007_set_gap(_disp, 0, 14);
  lv_display_set_rotation(_disp, LV_DISPLAY_ROTATION_270);
  lv_display_set_color_format(_disp, LV_COLOR_FORMAT_RGB565_SWAPPED);
  lv_display_set_buffers(_disp, _drawBuf, nullptr, sizeof(_drawBuf),
                         LV_DISPLAY_RENDER_MODE_PARTIAL);

  ui_init();
}

void Display::update() {
  lv_timer_handler();
  ui_tick();
}

// ── Private static callbacks ──────────────────────────
uint32_t Display::getTick() { return millis(); }

void Display::reset() {
  digitalWrite(PIN_LCD_RST, HIGH);
  delay(100);
  digitalWrite(PIN_LCD_RST, LOW);
  delay(120);
  digitalWrite(PIN_LCD_RST, HIGH);
  delay(120);
}

void Display::sendCmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size,
                      const uint8_t *param, size_t param_size) {
  SPI.beginTransaction(SPISettings(LCD_SPI_HZ, MSBFIRST, SPI_MODE0));
  digitalWrite(PIN_LCD_CS, LOW);
  digitalWrite(PIN_LCD_DC, LOW);
  SPI.transferBytes(cmd, nullptr, cmd_size);
  if (param && param_size > 0) {
    digitalWrite(PIN_LCD_DC, HIGH);
    SPI.transferBytes(param, nullptr, param_size);
  }
  digitalWrite(PIN_LCD_CS, HIGH);
  SPI.endTransaction();
}

void Display::sendColor(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size,
                        uint8_t *param, size_t param_size) {
  SPI.beginTransaction(SPISettings(LCD_SPI_HZ, MSBFIRST, SPI_MODE0));
  digitalWrite(PIN_LCD_CS, LOW);
  digitalWrite(PIN_LCD_DC, LOW);
  SPI.transferBytes(cmd, nullptr, cmd_size);
  if (param && param_size > 0) {
    digitalWrite(PIN_LCD_DC, HIGH);
    SPI.transferBytes(param, nullptr, param_size);
  }
  digitalWrite(PIN_LCD_CS, HIGH);
  SPI.endTransaction();
  lv_display_flush_ready(disp);
}
