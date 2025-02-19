#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "driver/i2s_std.h"
#include "i2s_manager.h"
#include <dirent.h>
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "file_manager.h"
#include "wav_player.h"


int current_sample_rate = SAMPLE_RATE;

static const char *TAG = "main";

#define STACK_SIZE      4096

// Add this define near the top with other defines
#define BUFFER_SIZE 1024

static esp_err_t audio_write_cb(const void* src, size_t size, void* user_data) {
    i2s_chan_handle_t tx_handle = (i2s_chan_handle_t)user_data;
    size_t bytes_written = 0;
    return i2s_channel_write(tx_handle, src, size, &bytes_written, portMAX_DELAY);
}

static esp_err_t play_wav_file(const char* filepath, void* user_data) {
    return wav_player_play_file(filepath, audio_write_cb, user_data);
}

void app_main(void)
{
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));
    ESP_ERROR_CHECK(esp_task_wdt_reset());

    ESP_LOGI(TAG, "Initializing SPIFFS");

    // Initialize file manager
    file_manager_config_t fm_config = {
        .base_path = "/storage",
        .partition_label = "storage",
        .max_files = 5,
        .format_if_mount_failed = true
    };

    esp_err_t ret = file_manager_init(&fm_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize file manager");
        return;
    }

    // Get SPIFFS information
    size_t total = 0, used = 0;
    ret = file_manager_get_info(&total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information");
        return;
    }
    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);

    // Initialize I2S
    i2s_chan_handle_t tx_handle;
    ret = init_i2s(&tx_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2S");
        return;
    }

    // Process WAV files
    ret = file_manager_process_wav_files(play_wav_file, tx_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error processing WAV files");
    }

    // Cleanup
    i2s_channel_disable(tx_handle);
    i2s_del_channel(tx_handle);
    file_manager_deinit();
    esp_task_wdt_delete(NULL);
    
    ESP_LOGI(TAG, "Playback complete");
}