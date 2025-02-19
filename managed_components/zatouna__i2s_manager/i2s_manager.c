/**
 * @file i2s_manager.c
 * @brief I2S (Inter-IC Sound) interface manager for audio output
 *
 * This module handles the configuration and initialization of the I2S interface
 * for audio output on ESP32. It sets up the I2S bus in master mode with
 * standard configuration for stereo audio output.
 */

#include "i2s_manager.h"
#include "esp_log.h"
#include "esp_check.h"

static const char *TAG = "I2S_CONFIG";

extern int current_sample_rate;

/**
 * @brief Initialize the I2S interface for audio output
 *
 * Configures and initializes an I2S channel in master mode with DMA support
 * for audio playback. The configuration includes:
 * - 16-bit stereo output
 * - DMA buffer settings for smooth playback
 * - GPIO pin assignments for BCK, LRCK, and DATA lines
 * - Sample rate based on current_sample_rate global variable
 *
 * @param[out] tx_handle Pointer to store the created I2S channel handle
 * @return esp_err_t ESP_OK on success, relevant error code on failure
 */
esp_err_t init_i2s(i2s_chan_handle_t *tx_handle) {
    i2s_chan_config_t chan_cfg = {
        .id = I2S_NUM_0,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = DMA_BUFFER_COUNT,
        .dma_frame_num = DMA_BUFFER_LEN,
        .auto_clear = true
    };
    
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(current_sample_rate),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = I2S_BCK_PIN,
            .ws = I2S_LRCK_PIN,
            .dout = I2S_DATA_PIN,
            .din = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    ESP_RETURN_ON_ERROR(i2s_new_channel(&chan_cfg, tx_handle, NULL), TAG, "Failed to create I2S channel");
    ESP_RETURN_ON_ERROR(i2s_channel_init_std_mode(*tx_handle, &std_cfg), TAG, "Failed to init I2S channel");
    ESP_RETURN_ON_ERROR(i2s_channel_enable(*tx_handle), TAG, "Failed to enable I2S channel");
    
    return ESP_OK;
} 