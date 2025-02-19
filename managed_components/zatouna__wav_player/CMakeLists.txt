idf_component_register(
    SRCS "wav_player.c"
    INCLUDE_DIRS "include"
    REQUIRES driver esp_timer freertos esp_common
)
