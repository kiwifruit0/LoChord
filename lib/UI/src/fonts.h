#ifndef EEZ_LVGL_UI_FONTS_H
#define EEZ_LVGL_UI_FONTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_font_t ui_font_press_start2_p_8;
extern const lv_font_t ui_font_press_start2_p_16;
extern const lv_font_t ui_font_press_start2_p_24;
extern const lv_font_t ui_font_press_start2_p_32;

#ifndef EXT_FONT_DESC_T
#define EXT_FONT_DESC_T
typedef struct _ext_font_desc_t {
    const char *name;
    const void *font_ptr;
} ext_font_desc_t;
#endif

extern ext_font_desc_t fonts[];

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_FONTS_H*/