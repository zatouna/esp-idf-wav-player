idf_component_register(SRCS "spiffs_example_main.c"
                       PRIV_REQUIRES spiffs driver i2s_manager file_manager wav_player
                       INCLUDE_DIRS "." )

