/**
 * @file file_manager.c
 * @brief File management system for SPIFFS filesystem
 *
 * This module provides functionality for managing files in the SPIFFS filesystem,
 * including initialization, file operations, and cleanup. It handles mounting
 * and unmounting of the filesystem, file information retrieval, and WAV file processing.
 *
 * @author Your Name
 * @date 2024
 */

#include <stdio.h>
#include "file_manager.h"
#include "esp_spiffs.h"
#include "esp_log.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * @brief Tag for ESP logging
 */
static const char *TAG = "file_manager";

/**
 * @brief Current SPIFFS partition label
 * 
 * Stores the partition label of the currently mounted filesystem.
 * NULL if no filesystem is mounted.
 */
static const char *current_partition_label = NULL;

/**
 * @brief Initialize and mount the SPIFFS filesystem
 * 
 * Configures SPIFFS parameters and attempts to mount the filesystem.
 * If mounting fails and format_if_mount_failed is true, formats the partition
 * and attempts to mount again.
 * 
 * @param config Pointer to configuration structure containing:
 *        - base_path: Mount point in VFS
 *        - partition_label: Label of SPIFFS partition
 *        - max_files: Maximum number of files that can be open simultaneously
 *        - format_if_mount_failed: Whether to format if mounting fails
 * 
 * @return ESP_OK on successful mount
 *         ESP_ERR_INVALID_ARG if config is NULL
 *         ESP_FAIL if mounting fails and format is disabled or fails
 */
esp_err_t file_manager_init(const file_manager_config_t *config)
{
    if (config == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    esp_vfs_spiffs_conf_t conf = {
        .base_path = config->base_path,
        .partition_label = config->partition_label,
        .max_files = config->max_files,
        .format_if_mount_failed = config->format_if_mount_failed};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to mount SPIFFS (%s)", esp_err_to_name(ret));
        return ret;
    }

    current_partition_label = config->partition_label;
    ESP_LOGI(TAG, "SPIFFS mounted successfully");
    return ESP_OK;
}

/**
 * @brief Get filesystem information
 * 
 * Retrieves total and used space information from the mounted filesystem.
 * Must be called after successful initialization.
 * 
 * @param[out] total_bytes Pointer to store total size in bytes
 * @param[out] used_bytes Pointer to store used space in bytes
 * 
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_STATE if no filesystem is mounted
 *         ESP_FAIL if information cannot be retrieved
 */
esp_err_t file_manager_get_info(size_t *total_bytes, size_t *used_bytes)
{
    if (!current_partition_label)
    {
        return ESP_ERR_INVALID_STATE;
    }
    return esp_spiffs_info(current_partition_label, total_bytes, used_bytes);
}

/**
 * @brief Process WAV files in the storage directory
 * 
 * Opens the /storage directory and iterates through all files.
 * For each file with .wav extension:
 * 1. Constructs the full filepath
 * 2. Logs the filename
 * 3. Calls the provided callback with the filepath
 * 
 * @param callback Function pointer to process each WAV file
 *        The callback should accept:
 *        - const char *filepath: Path to the WAV file
 *        - void *user_data: User-provided data
 * @param user_data User data passed to callback
 * 
 * @return ESP_OK if all files processed successfully
 *         ESP_FAIL if directory cannot be opened
 *         Returns callback's return value if it fails
 */
esp_err_t file_manager_process_wav_files(esp_err_t (*callback)(const char *filepath, void *user_data), void *user_data)
{
    DIR *dir = opendir("/storage");
    if (dir == NULL)
    {
        ESP_LOGE(TAG, "Failed to open directory");
        return ESP_FAIL;
    }

    struct dirent *ent;
    esp_err_t ret = ESP_OK;

    while ((ent = readdir(dir)) != NULL)
    {
        if (strstr(ent->d_name, ".wav") != NULL)
        {
            char filepath[300];
            if (strlen(ent->d_name) + strlen("/storage/") >= sizeof(filepath))
            {
                ESP_LOGE(TAG, "Filename too long: %s", ent->d_name);
                continue;
            }

            snprintf(filepath, sizeof(filepath), "/storage/%s", ent->d_name);
            ESP_LOGI(TAG, "Processing file: %s", ent->d_name);

            ret = callback(filepath, user_data);
            if (ret != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to process file: %s", ent->d_name);
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }

    closedir(dir);
    return ret;
}

/**
 * @brief Unmount filesystem and cleanup
 * 
 * Unmounts the SPIFFS filesystem if mounted and clears the partition label.
 * Safe to call even if filesystem is not mounted.
 * 
 * This function should be called:
 * - Before system shutdown
 * - When filesystem access is no longer needed
 * - Before remounting with different parameters
 * 
 * @note This function is idempotent and can be safely called multiple times
 */
void file_manager_deinit(void)
{
    if (current_partition_label)
    {
        esp_vfs_spiffs_unregister(current_partition_label);
        current_partition_label = NULL;
        ESP_LOGI(TAG, "SPIFFS unmounted");
    }
}

