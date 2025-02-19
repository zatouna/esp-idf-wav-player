#ifndef I2S_MANAGER_H
#define I2S_MANAGER_H

#include "driver/i2s_std.h"
#include "esp_err.h"

// Add these definitions
#define SAMPLE_RATE     44100
#define I2S_BCK_PIN     15
#define I2S_LRCK_PIN    16
#define I2S_DATA_PIN    17

// Add DMA buffer definitions
#define DMA_BUFFER_COUNT 8
#define DMA_BUFFER_LEN   1024

// Function declarations
/**
 * @brief Initialize I2S interface
 * 
 * Configures and initializes the I2S interface for audio output.
 * Sets up DMA buffers and configures pins for audio transmission.
 * 
 * @param[out] tx_handle Pointer to store the I2S channel handle
 * @return ESP_OK on successful initialization
 *         ESP_FAIL if initialization fails
 *         ESP_ERR_NO_MEM if memory allocation fails
 *         ESP_ERR_INVALID_ARG if tx_handle is NULL
 */
esp_err_t init_i2s(i2s_chan_handle_t *tx_handle);

/**
 * @brief Configure I2S parameters
 * 
 * Sets the sample rate, bit depth, and channel format for I2S transmission.
 * Should be called after initialization and before starting transmission.
 * 
 * @param tx_handle I2S channel handle
 * @param sample_rate Sample rate in Hz (e.g., 44100, 48000)
 * @param bits_per_sample Bit depth (16 or 24)
 * @param num_channels Number of channels (1 for mono, 2 for stereo)
 * @return ESP_OK on successful configuration
 *         ESP_FAIL if configuration fails
 *         ESP_ERR_INVALID_ARG if parameters are invalid
 */
esp_err_t configure_i2s(i2s_chan_handle_t tx_handle, uint32_t sample_rate, uint16_t bits_per_sample, uint8_t num_channels);

/**
 * @brief Start I2S transmission
 * 
 * Enables the I2S peripheral and starts DMA transfers.
 * 
 * @param tx_handle I2S channel handle
 * @return ESP_OK on success
 *         ESP_FAIL if start fails
 */
esp_err_t start_i2s(i2s_chan_handle_t tx_handle);

/**
 * @brief Stop I2S transmission
 * 
 * Stops DMA transfers and disables the I2S peripheral.
 * 
 * @param tx_handle I2S channel handle
 * @return ESP_OK on success
 *         ESP_FAIL if stop fails
 */
esp_err_t stop_i2s(i2s_chan_handle_t tx_handle);

#endif // I2S_MANAGER_H 