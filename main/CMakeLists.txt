idf_component_register(SRCS "alarm_clock.c" "espnow_example_main.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_wifi nvs_flash)
