#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

/**
 * @brief WAV file header structure containing audio format information
 */
typedef struct {
    uint16_t num_channels;      /**< Number of audio channels (1=mono, 2=stereo) */
    uint32_t sample_rate;       /**< Sample rate in Hz (e.g., 44100, 48000) */
    uint16_t bits_per_sample;   /**< Bits per sample (16 or 24) */
    uint32_t data_size;         /**< Size of audio data in bytes */
    uint16_t block_align;       /**< Block alignment (channels * bits_per_sample / 8) */
} wav_header_t;

// Volume control
#define MIN_VOLUME 0
#define MAX_VOLUME 100
#define BUFFER_SIZE 1024

/**
 * @brief Callback function type for writing audio data
 * @param src Pointer to source audio data buffer
 * @param size Size of the audio data in bytes
 * @param user_data User-provided context data
 * @return ESP_OK on success, ESP_FAIL on failure
 */
typedef esp_err_t (*wav_player_write_cb_t)(const void* src, size_t size, void* user_data);

/**
 * @brief Play a WAV file using the provided write callback
 * 
 * This function reads a WAV file and streams the audio data through the provided callback.
 * Supports both 16-bit and 24-bit audio in mono or stereo format.
 * 
 * @param filepath Path to the WAV file to play
 * @param write_cb Callback function that will receive the audio data
 * @param user_data User data that will be passed to the callback
 * @return ESP_OK on successful playback
 *         ESP_ERR_INVALID_ARG if write_cb is NULL
 *         ESP_FAIL if file cannot be opened or has invalid format
 */
esp_err_t wav_player_play_file(const char* filepath, wav_player_write_cb_t write_cb, void* user_data);

/**
 * @brief Get information about a WAV file
 * 
 * Reads the WAV header and provides format information without playing the file.
 * 
 * @param filepath Path to the WAV file
 * @param header Pointer to wav_header_t structure to store the information
 * @return ESP_OK on success
 *         ESP_FAIL if file cannot be opened or header is invalid
 */
esp_err_t wav_player_get_info(const char* filepath, wav_header_t* header);

/**
 * @brief Set the playback volume
 * 
 * Sets the global volume level for audio playback.
 * 
 * @param volume Volume level from 0 (mute) to 100 (full volume)
 *               Values outside this range will be clamped
 */
void wav_player_set_volume(int volume);

/**
 * @brief Increase the playback volume
 * 
 * Increases the current volume level by the specified amount.
 * 
 * @param amount Amount to increase the volume (0-100)
 *               Will be clamped to maximum volume of 100
 */
void wav_player_increase_volume(int amount);

/**
 * @brief Decrease the playback volume
 * 
 * Decreases the current volume level by the specified amount.
 * 
 * @param amount Amount to decrease the volume (0-100)
 *               Will be clamped to minimum volume of 0
 */
void wav_player_decrease_volume(int amount);

/**
 * @brief Get current volume level
 * 
 * @return Current volume level (0-100)
 */
int wav_player_get_volume(void);
