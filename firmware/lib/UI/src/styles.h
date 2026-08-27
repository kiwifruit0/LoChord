#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: text-medium
lv_style_t *get_style_text_medium_MAIN_DEFAULT();
void add_style_text_medium(lv_obj_t *obj);
void remove_style_text_medium(lv_obj_t *obj);

// Style: text-small
lv_style_t *get_style_text_small_MAIN_DEFAULT();
void add_style_text_small(lv_obj_t *obj);
void remove_style_text_small(lv_obj_t *obj);

// Style: text-tiny
lv_style_t *get_style_text_tiny_MAIN_DEFAULT();
void add_style_text_tiny(lv_obj_t *obj);
void remove_style_text_tiny(lv_obj_t *obj);

// Style: SettingsMatrix
lv_style_t *get_style_settings_matrix_MAIN_DEFAULT();
lv_style_t *get_style_settings_matrix_ITEMS_DEFAULT();
lv_style_t *get_style_settings_matrix_ITEMS_CHECKED();
void add_style_settings_matrix(lv_obj_t *obj);
void remove_style_settings_matrix(lv_obj_t *obj);

// Style: ChordMatrix
lv_style_t *get_style_chord_matrix_MAIN_DEFAULT();
lv_style_t *get_style_chord_matrix_ITEMS_DEFAULT();
lv_style_t *get_style_chord_matrix_ITEMS_CHECKED();
void add_style_chord_matrix(lv_obj_t *obj);
void remove_style_chord_matrix(lv_obj_t *obj);

// Style: settingsPanel
lv_style_t *get_style_settings_panel_MAIN_DEFAULT();
void add_style_settings_panel(lv_obj_t *obj);
void remove_style_settings_panel(lv_obj_t *obj);

// Style: settingsCell
lv_style_t *get_style_settings_cell_MAIN_DEFAULT();
void add_style_settings_cell(lv_obj_t *obj);
void remove_style_settings_cell(lv_obj_t *obj);

// Style: settingsContainer
lv_style_t *get_style_settings_container_MAIN_DEFAULT();
lv_style_t *get_style_settings_container_SCROLLBAR_DEFAULT();
void add_style_settings_container(lv_obj_t *obj);
void remove_style_settings_container(lv_obj_t *obj);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/