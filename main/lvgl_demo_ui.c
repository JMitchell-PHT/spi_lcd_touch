/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

// This demo UI is adapted from LVGL official example: https://docs.lvgl.io/master/examples.html#loader-with-arc

#include "lvgl.h"
#include "esp_log.h"

static lv_obj_t * btn;
static lv_display_rotation_t rotation = LV_DISP_ROTATION_0;

static void btn_cb(lv_event_t * e)
{
    lv_display_t *disp = lv_event_get_user_data(e);
    rotation++;
    if (rotation > LV_DISP_ROTATION_270) {
        rotation = LV_DISP_ROTATION_0;
    }
    lv_disp_set_rotation(disp, rotation);
}

static void roller_event_handler(lv_event_t *e)
{
    lv_obj_t *roller = lv_event_get_target(e);
    uint16_t selected = lv_roller_get_selected(roller);
    ESP_LOGI("ROLLER", "Selected option: %d", selected);
}

void create_roller(void)
{
    // Create roller
    lv_obj_t *roller = lv_roller_create(lv_screen_active());
    
    // Set options (comma-separated list)
    lv_roller_set_options(roller, 
        "Option 1\n"
        "Option 2\n"
        "Option 3\n"
        "Option 4\n"
        "Option 5", 
        LV_ROLLER_MODE_NORMAL); // Use INFINITE for continuous scrolling
    
    // Set properties
    lv_roller_set_visible_row_count(roller, 3); // How many options are visible at once
    lv_obj_align(roller, LV_ALIGN_CENTER, 0, 0);

    // Attach event
    lv_obj_add_event_cb(roller, roller_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
}


void example_lvgl_demo_ui(lv_display_t *disp)
{
    lv_obj_t *scr = lv_display_get_screen_active(disp);

    create_roller(); // Add roller to the UI

    btn = lv_button_create(scr);
    lv_obj_t * lbl = lv_label_create(btn);
    lv_label_set_text_static(lbl, LV_SYMBOL_REFRESH" ROTATE");
    lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 30, -30);
    /*Button event*/
    lv_obj_add_event_cb(btn, btn_cb, LV_EVENT_CLICKED, disp);
}
