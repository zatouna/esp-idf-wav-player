#include <stdio.h>
#include "wav_player.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "wav_player";
#define BUFFER_SIZE 1024

static int current_volume = 30;  // Default volume (0-100)

/**
 * @brief Convert 24-bit audio sample to 16-bit
 * 
 * Converts a 24-bit audio sample to 16-bit format while preserving sign.
 * 
 * @param sample_24bit Pointer to 3-byte array containing 24-bit sample
 * @return Converted 16-bit sample
 */
static inline int16_t convert_24_to_16(const uint8_t* sample_24bit) {
    int32_t sample_24 = (sample_24bit[2] << 16) | (sample_24bit[1] << 8) | sample_24bit[0];
    
    if (sample_24 & 0x800000) {
        sample_24 |= 0xFF000000;
    }
    
    return (int16_t)(sample_24 >> 8);
}

/**
 * @brief Apply volume adjustment to audio sample
 * 
 * Scales a 16-bit audio sample according to the current volume setting.
 * 
 * @param sample Original 16-bit audio sample
 * @param volume Volume level (0-100)
 * @return Volume-adjusted sample
 */
static inline int16_t apply_volume(int16_t sample, int volume) {
    float volume_multiplier = volume / 100.0f;
    return (int16_t)(sample * volume_multiplier);
}

/**
 * @brief Validate WAV header format
 * 
 * Checks if the WAV header contains valid and supported format information.
 * Supported formats:
 * - 1 or 2 channels
 * - 16 or 24 bits per sample
 * - Sample rates between 8000 and 48000 Hz
 * 
 * @param header Pointer to WAV header structure
 * @return true if format is valid and supported
 *         false if format is invalid or unsupported
 */
static bool is_valid_wav_header(const wav_header_t* header) {
    if (header->num_channels < 1 || header->num_channels > 2) {
        ESP_LOGE(TAG, "Unsupported number of channels: %d", header->num_channels);
        return false;
    }

    if (header->bits_per_sample != 16 && header->bits_per_sample != 24) {
        ESP_LOGE(TAG, "Unsupported bits per sample: %d", header->bits_per_sample);
        return false;
    }

    if (header->sample_rate < 8000 || header->sample_rate > 48000) {
        ESP_LOGE(TAG, "Invalid sample rate: %lu", header->sample_rate);
        return false;
    }

    uint16_t expected_block_align = header->num_channels * (header->bits_per_sample / 8);
    if (header->block_align != expected_block_align) {
        ESP_LOGE(TAG, "Invalid block align: %d (expected %d)", 
                 header->block_align, expected_block_align);
        return false;
    }

    return true;
}

/**
 * @brief Read WAV file header
 * 
 * Reads and parses the WAV file header into the header structure.
 * 
 * @param f File pointer to open WAV file
 * @param header Pointer to header structure to fill
 * @return ESP_OK on successful read
 *         ESP_FAIL if read fails
 */
static esp_err_t read_wav_header(FILE* f, wav_header_t* header) {
    uint8_t wav_header[44];
    
    if (fread(wav_header, 1, 44, f) != 44) {
        ESP_LOGE(TAG, "Failed to read WAV header");
        return ESP_FAIL;
    }

    header->num_channels = wav_header[22] | (wav_header[23] << 8);
    header->sample_rate = wav_header[24] | (wav_header[25] << 8) | 
                         (wav_header[26] << 16) | (wav_header[27] << 24);
    header->bits_per_sample = wav_header[34] | (wav_header[35] << 8);
    header->block_align = wav_header[32] | (wav_header[33] << 8);
    header->data_size = wav_header[40] | (wav_header[41] << 8) | 
                       (wav_header[42] << 16) | (wav_header[43] << 24);

    return ESP_OK;
}

esp_err_t wav_player_get_info(const char* filepath, wav_header_t* header) {
    FILE* fp = fopen(filepath, "rb");
    if (fp == NULL) {
        ESP_LOGE(TAG, "Failed to open file %s", filepath);
        return ESP_FAIL;
    }

    esp_err_t ret = read_wav_header(fp, header);
    fclose(fp);
    return ret;
}

esp_err_t wav_player_play_file(const char* filepath, wav_player_write_cb_t write_cb, void* user_data) {
    if (write_cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    FILE* fp = fopen(filepath, "rb");
    if (fp == NULL) {
        ESP_LOGE(TAG, "Failed to open file %s", filepath);
        return ESP_FAIL;
    }

    wav_header_t wav_header;
    if (read_wav_header(fp, &wav_header) != ESP_OK || !is_valid_wav_header(&wav_header)) {
        ESP_LOGE(TAG, "Invalid WAV header");
        fclose(fp);
        return ESP_FAIL;
    }

    uint8_t *buffer = malloc(BUFFER_SIZE);
    uint8_t *processed_buffer = malloc(BUFFER_SIZE);
    if (buffer == NULL || processed_buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate buffers");
        free(buffer);
        free(processed_buffer);
        fclose(fp);
        return ESP_FAIL;
    }

    size_t bytes_read;
    esp_err_t ret = ESP_OK;
    
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        // Process audio data with volume adjustment
        if (wav_header.bits_per_sample == 16) {
            int16_t *samples = (int16_t*)buffer;
            int16_t *processed = (int16_t*)processed_buffer;
            size_t sample_count = bytes_read / 2;
            
            for (size_t i = 0; i < sample_count; i++) {
                processed[i] = apply_volume(samples[i], current_volume);
            }
        } else if (wav_header.bits_per_sample == 24) {
            // Process 24-bit audio in chunks of 3 bytes
            for (size_t i = 0; i < bytes_read; i += 3) {
                int16_t sample = convert_24_to_16(&buffer[i]);
                int16_t processed = apply_volume(sample, current_volume);
                // Convert back to 24-bit if needed
                // ... conversion code here ...
            }
        }

        ret = write_cb(processed_buffer, bytes_read, user_data);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Write callback failed");
            break;
        }
    }

    free(processed_buffer);
    free(buffer);
    fclose(fp);
    return ret;
}

void wav_player_set_volume(int volume) {
    if (volume < MIN_VOLUME) volume = MIN_VOLUME;
    if (volume > MAX_VOLUME) volume = MAX_VOLUME;
    current_volume = volume;
    ESP_LOGI(TAG, "Volume set to %d%%", current_volume);
}

void wav_player_increase_volume(int amount) {
    wav_player_set_volume(current_volume + amount);
}

void wav_player_decrease_volume(int amount) {
    wav_player_set_volume(current_volume - amount);
}

int wav_player_get_volume(void) {
    return current_volume;
}
