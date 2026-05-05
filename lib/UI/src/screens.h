#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_MAIN = 1,
    _SCREEN_ID_LAST = 1
};

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *obj0;
    lv_obj_t *chord_root_label;
    lv_obj_t *chord_quality_label;
    lv_obj_t *chord_notes_label;
    lv_obj_t *obj1;
    lv_obj_t *chord_matrix;
    lv_obj_t *obj2;
    lv_obj_t *bpm_label;
    lv_obj_t *obj3;
    lv_obj_t *channel_label;
    lv_obj_t *obj4;
    lv_obj_t *clock_label;
    lv_obj_t *ready_label;
    lv_obj_t *usb_label;
} objects_t;

extern objects_t objects;

void create_screen_main();
void tick_screen_main();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

// Color themes

enum Themes {
    THEME_ID_DEFAULT,
    THEME_ID_PURPLE_DARK,
    THEME_ID_NORD,
};
enum Colors {
    COLOR_ID_BACKGROUND,
    COLOR_ID_DARK1,
    COLOR_ID_DARK2,
    COLOR_ID_GREY1,
    COLOR_ID_LIGHT1,
    COLOR_ID_ACCENT1,
    COLOR_ID_ACCENT2,
    COLOR_ID_ALT1,
    COLOR_ID_ALT2,
    COLOR_ID_ALT3,
};
void change_color_theme(uint32_t themeIndex);
extern uint32_t theme_colors[3][10];
extern uint32_t active_theme_index;

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/