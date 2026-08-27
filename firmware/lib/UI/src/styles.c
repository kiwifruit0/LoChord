#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: text-medium
//

void init_style_text_medium_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &ui_font_press_start2_p_24);
};

lv_style_t *get_style_text_medium_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_text_medium_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_text_medium(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_text_medium_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_text_medium(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_text_medium_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: text-small
//

void init_style_text_small_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &ui_font_press_start2_p_16);
};

lv_style_t *get_style_text_small_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_text_small_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_text_small(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_text_small_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_text_small(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_text_small_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: text-tiny
//

void init_style_text_tiny_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &ui_font_press_start2_p_8);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
};

lv_style_t *get_style_text_tiny_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_text_tiny_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_text_tiny(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_text_tiny_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_text_tiny(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_text_tiny_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: SettingsMatrix
//

void init_style_settings_matrix_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_pad_row(style, 0);
    lv_style_set_pad_column(style, 0);
    lv_style_set_radius(style, 0);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_border_color(style, lv_color_hex(0x2f3237));
    lv_style_set_border_width(style, 0);
    {
        static lv_coord_t dsc[] = {0, LV_GRID_TEMPLATE_LAST};
        lv_style_set_grid_row_dsc_array(style, dsc);
    }
    {
        static lv_coord_t dsc[] = {0, LV_GRID_TEMPLATE_LAST};
        lv_style_set_grid_column_dsc_array(style, dsc);
    }
};

lv_style_t *get_style_settings_matrix_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_matrix_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_settings_matrix_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_radius(style, 0);
    lv_style_set_text_font(style, &ui_font_press_start2_p_8);
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_text_align(style, LV_TEXT_ALIGN_LEFT);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_pad_row(style, 0);
    lv_style_set_pad_column(style, 0);
    lv_style_set_outline_width(style, 1);
    lv_style_set_outline_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
};

lv_style_t *get_style_settings_matrix_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_matrix_ITEMS_DEFAULT(style);
    }
    return style;
};

void init_style_settings_matrix_ITEMS_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][4]));
};

lv_style_t *get_style_settings_matrix_ITEMS_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_matrix_ITEMS_CHECKED(style);
    }
    return style;
};

void add_style_settings_matrix(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_settings_matrix_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_settings_matrix_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_settings_matrix_ITEMS_CHECKED(), LV_PART_ITEMS | LV_STATE_CHECKED);
};

void remove_style_settings_matrix(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_settings_matrix_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_settings_matrix_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_settings_matrix_ITEMS_CHECKED(), LV_PART_ITEMS | LV_STATE_CHECKED);
};

//
// Style: ChordMatrix
//

void init_style_chord_matrix_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_pad_row(style, 5);
    lv_style_set_pad_column(style, 5);
    lv_style_set_radius(style, 0);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
};

lv_style_t *get_style_chord_matrix_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_chord_matrix_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_chord_matrix_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_radius(style, 0);
    lv_style_set_text_font(style, &ui_font_press_start2_p_8);
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_border_width(style, 2);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
};

lv_style_t *get_style_chord_matrix_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_chord_matrix_ITEMS_DEFAULT(style);
    }
    return style;
};

void init_style_chord_matrix_ITEMS_CHECKED(lv_style_t *style) {
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_border_width(style, 2);
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
};

lv_style_t *get_style_chord_matrix_ITEMS_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_chord_matrix_ITEMS_CHECKED(style);
    }
    return style;
};

void add_style_chord_matrix(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_chord_matrix_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_chord_matrix_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_chord_matrix_ITEMS_CHECKED(), LV_PART_ITEMS | LV_STATE_CHECKED);
};

void remove_style_chord_matrix(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_chord_matrix_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_chord_matrix_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_chord_matrix_ITEMS_CHECKED(), LV_PART_ITEMS | LV_STATE_CHECKED);
};

//
// Style: settingsPanel
//

void init_style_settings_panel_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_radius(style, 0);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_border_width(style, 1);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_pad_row(style, 0);
    lv_style_set_pad_column(style, 0);
};

lv_style_t *get_style_settings_panel_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_panel_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_settings_panel(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_settings_panel_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_settings_panel(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_settings_panel_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: settingsCell
//

void init_style_settings_cell_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_grid_cell_row_span(style, 1);
    lv_style_set_grid_cell_column_span(style, 1);
};

lv_style_t *get_style_settings_cell_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_cell_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_settings_cell(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_settings_cell_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_settings_cell(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_settings_cell_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: settingsContainer
//

void init_style_settings_container_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_layout(style, LV_LAYOUT_GRID);
    {
        static lv_coord_t dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
        lv_style_set_grid_row_dsc_array(style, dsc);
    }
    {
        static lv_coord_t dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), 2, LV_GRID_TEMPLATE_LAST};
        lv_style_set_grid_column_dsc_array(style, dsc);
    }
    lv_style_set_pad_row(style, 1);
    lv_style_set_pad_column(style, 1);
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_pad_top(style, 1);
    lv_style_set_pad_bottom(style, 1);
    lv_style_set_pad_left(style, 1);
    lv_style_set_pad_right(style, 1);
};

lv_style_t *get_style_settings_container_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_container_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_settings_container_SCROLLBAR_DEFAULT(lv_style_t *style) {
    lv_style_set_width(style, 3);
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_radius(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_opa(style, 255);
};

lv_style_t *get_style_settings_container_SCROLLBAR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_container_SCROLLBAR_DEFAULT(style);
    }
    return style;
};

void add_style_settings_container(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_settings_container_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_settings_container_SCROLLBAR_DEFAULT(), LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
};

void remove_style_settings_container(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_settings_container_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_settings_container_SCROLLBAR_DEFAULT(), LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_text_medium,
        add_style_text_small,
        add_style_text_tiny,
        add_style_settings_matrix,
        add_style_chord_matrix,
        add_style_settings_panel,
        add_style_settings_cell,
        add_style_settings_container,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_text_medium,
        remove_style_text_small,
        remove_style_text_tiny,
        remove_style_settings_matrix,
        remove_style_chord_matrix,
        remove_style_settings_panel,
        remove_style_settings_cell,
        remove_style_settings_container,
    };
    remove_style_funcs[styleIndex](obj);
}