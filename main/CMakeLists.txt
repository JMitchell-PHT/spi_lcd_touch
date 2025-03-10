idf_component_register(
    SRCS "spi_lcd_touch_example_main.c"
         "lvgl_demo_ui.c"
         "FT6X36.c"
    INCLUDE_DIRS "." "include"
    REQUIRES driver lvgl 
)