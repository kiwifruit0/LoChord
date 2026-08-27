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
    SCREEN_ID_SETTINGS = 2,
    _SCREEN_ID_LAST = 2
};

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *settings;
    lv_obj_t *obj0;
    lv_obj_t *chord_container;
    lv_obj_t *chord_root_label;
    lv_obj_t *chord_quality_label;
    lv_obj_t *chord_notes_label;
    lv_obj_t *joystick_container;
    lv_obj_t *obj1;
    lv_obj_t *joystick_matrix;
    lv_obj_t *info_container;
    lv_obj_t *bpm_label;
    lv_obj_t *key_label;
    lv_obj_t *arp_label;
    lv_obj_t *strum_label;
    lv_obj_t *ready_label;
    lv_obj_t *usb_label;
    lv_obj_t *obj2;
    lv_obj_t *chord_container_settings;
    lv_obj_t *chord_root_label_1;
    lv_obj_t *chord_quality_label_1;
    lv_obj_t *chord_notes_label_1;
    lv_obj_t *settings_container;
    lv_obj_t *settings_cell1;
    lv_obj_t *settings_cell1__obj0;
    lv_obj_t *settings_cell1__settings_category_label;
    lv_obj_t *settings_cell1__settings_type_label;
    lv_obj_t *settings_cell1__settings_value_label;
    lv_obj_t *settings_cell2;
    lv_obj_t *settings_cell2__obj0;
    lv_obj_t *settings_cell2__settings_category_label;
    lv_obj_t *settings_cell2__settings_type_label;
    lv_obj_t *settings_cell2__settings_value_label;
    lv_obj_t *settings_cell3;
    lv_obj_t *settings_cell3__obj0;
    lv_obj_t *settings_cell3__settings_category_label;
    lv_obj_t *settings_cell3__settings_type_label;
    lv_obj_t *settings_cell3__settings_value_label;
    lv_obj_t *settings_cell4;
    lv_obj_t *settings_cell4__obj0;
    lv_obj_t *settings_cell4__settings_category_label;
    lv_obj_t *settings_cell4__settings_type_label;
    lv_obj_t *settings_cell4__settings_value_label;
    lv_obj_t *settings_cell5;
    lv_obj_t *settings_cell5__obj0;
    lv_obj_t *settings_cell5__settings_category_label;
    lv_obj_t *settings_cell5__settings_type_label;
    lv_obj_t *settings_cell5__settings_value_label;
    lv_obj_t *settings_cell6;
    lv_obj_t *settings_cell6__obj0;
    lv_obj_t *settings_cell6__settings_category_label;
    lv_obj_t *settings_cell6__settings_type_label;
    lv_obj_t *settings_cell6__settings_value_label;
    lv_obj_t *settings_cell7;
    lv_obj_t *settings_cell7__obj0;
    lv_obj_t *settings_cell7__settings_category_label;
    lv_obj_t *settings_cell7__settings_type_label;
    lv_obj_t *settings_cell7__settings_value_label;
    lv_obj_t *settings_cell8;
    lv_obj_t *settings_cell8__obj0;
    lv_obj_t *settings_cell8__settings_category_label;
    lv_obj_t *settings_cell8__settings_type_label;
    lv_obj_t *settings_cell8__settings_value_label;
    lv_obj_t *settings_cell9;
    lv_obj_t *settings_cell9__obj0;
    lv_obj_t *settings_cell9__settings_category_label;
    lv_obj_t *settings_cell9__settings_type_label;
    lv_obj_t *settings_cell9__settings_value_label;
    lv_obj_t *settings_cell10;
    lv_obj_t *settings_cell10__obj0;
    lv_obj_t *settings_cell10__settings_category_label;
    lv_obj_t *settings_cell10__settings_type_label;
    lv_obj_t *settings_cell10__settings_value_label;
    lv_obj_t *settings_cell11;
    lv_obj_t *settings_cell11__obj0;
    lv_obj_t *settings_cell11__settings_category_label;
    lv_obj_t *settings_cell11__settings_type_label;
    lv_obj_t *settings_cell11__settings_value_label;
    lv_obj_t *settings_cell12;
    lv_obj_t *settings_cell12__obj0;
    lv_obj_t *settings_cell12__settings_category_label;
    lv_obj_t *settings_cell12__settings_type_label;
    lv_obj_t *settings_cell12__settings_value_label;
    lv_obj_t *settings_cell13;
    lv_obj_t *settings_cell13__obj0;
    lv_obj_t *settings_cell13__settings_category_label;
    lv_obj_t *settings_cell13__settings_type_label;
    lv_obj_t *settings_cell13__settings_value_label;
} objects_t;

extern objects_t objects;

void create_screen_main();
void tick_screen_main();

void create_screen_settings();
void tick_screen_settings();

void create_user_widget_settings_cell(lv_obj_t *parent_obj, int startWidgetIndex);
void tick_user_widget_settings_cell(int startWidgetIndex);

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