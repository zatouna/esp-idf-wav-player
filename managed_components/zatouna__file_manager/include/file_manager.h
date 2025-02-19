#pragma once

#include "esp_err.h"
#include <dirent.h>
#include <stdbool.h>

/**
 * @brief Configuration structure for the file manager
 * 
 * Contains all necessary parameters to initialize the file system.
 */
typedef struct {
    const char *base_path;        /**< Base path for SPIFFS (e.g., "/storage") */
    const char *partition_label;   /**< Partition label (e.g., "storage") */
    size_t max_files;             /**< Maximum number of files that can be open simultaneously */
    bool format_if_mount_failed;   /**< Whether to format the filesystem if mounting fails */
} file_manager_config_t;

/**
 * @brief Initialize the file system
 * 
 * Initializes and mounts the SPIFFS file system using the provided configuration.
 * If mounting fails and format_if_mount_failed is true, will attempt to format
 * the partition before mounting again.
 * 
 * @param config Pointer to configuration structure
 * @return ESP_OK on successful initialization
 *         ESP_FAIL if mounting fails and format is disabled or fails
 *         ESP_ERR_INVALID_ARG if config is NULL or contains invalid values
 */
esp_err_t file_manager_init(const file_manager_config_t *config);

/**
 * @brief Get SPIFFS partition information
 * 
 * Retrieves the total and used space information for the mounted SPIFFS partition.
 * 
 * @param total_bytes Pointer to store total partition size in bytes
 * @param used_bytes Pointer to store used space in bytes
 * @return ESP_OK on success
 *         ESP_FAIL if file system is not mounted or info cannot be retrieved
 *         ESP_ERR_INVALID_ARG if either pointer is NULL
 */
esp_err_t file_manager_get_info(size_t *total_bytes, size_t *used_bytes);

/**
 * @brief Process each WAV file in the specified directory
 * 
 * Iterates through all files in the mounted filesystem and calls the provided
 * callback function for each WAV file found. Files are processed in alphabetical order.
 * 
 * @param callback Function to call for each WAV file found
 *                 Callback receives filepath and user_data as parameters
 *                 Should return ESP_OK to continue processing, any other value to stop
 * @param user_data User data pointer passed to callback function
 * @return ESP_OK if all files were processed successfully
 *         ESP_FAIL if directory cannot be opened or read
 *         ESP_ERR_INVALID_ARG if callback is NULL
 */
esp_err_t file_manager_process_wav_files(esp_err_t (*callback)(const char *filepath, void *user_data), void *user_data);

/**
 * @brief Cleanup and unmount the file system
 * 
 * Unmounts the SPIFFS filesystem and frees associated resources.
 * Should be called when the file system is no longer needed.
 */
void file_manager_deinit(void);
