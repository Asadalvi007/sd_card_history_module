/**
 * @file sd_card_example_main.c
 * @author Engr. Muhammad Asad Ali (asadalvi811@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdspi_host.h"

static const char *TAG = "SDCard";
#define MOUNT_POINT "/sdcard"

// Pin configuration (set via menuconfig)
#define PIN_NUM_MISO CONFIG_EXAMPLE_PIN_MISO
#define PIN_NUM_MOSI CONFIG_EXAMPLE_PIN_MOSI
#define PIN_NUM_CLK CONFIG_EXAMPLE_PIN_CLK
#define PIN_NUM_CS CONFIG_EXAMPLE_PIN_CS

static uint32_t history_counter = 1;

// Function to write data to the SD card
esp_err_t write_data_to_sd_card(const char *file_path, const uint8_t *data, size_t size)
{
    FILE *f = fopen(file_path, "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing: %s", file_path);
        return ESP_FAIL;
    }

    size_t bytes_written = fwrite(data, 1, size, f);
    fclose(f);

    if (bytes_written != size)
    {
        ESP_LOGE(TAG, "Failed to write all data to file: %s", file_path);
        return ESP_FAIL;
    }

    return ESP_OK;
}

// Function to read data from the SD card
esp_err_t read_data_from_sd_card(const char *file_path, uint8_t *data, size_t size)
{
    FILE *f = fopen(file_path, "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading: %s", file_path);
        return ESP_FAIL;
    }

    size_t bytes_read = fread(data, 1, size, f);
    fclose(f);

    if (bytes_read != size)
    {
        ESP_LOGE(TAG, "Failed to read all data from file: %s", file_path);
        return ESP_FAIL;
    }

    return ESP_OK;
}

// Initialize SD card and mount the filesystem
esp_err_t initialize_sd_card(sdmmc_card_t **card)
{
    // SD card mount configuration
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};

    // SPI bus configuration
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000};

    esp_err_t ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize SPI bus.");
        return ret;
    }

    // SD card slot configuration
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    // Mount the SD card
    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, &mount_config, card);
    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount filesystem. Format the card if mount fails by enabling CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED.");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize the card: %s. Check pull-up resistors on SD card lines.", esp_err_to_name(ret));
        }
        return ret;
    }

    ESP_LOGI(TAG, "Filesystem mounted successfully");
    return ESP_OK;
}

// Print SD card information
void print_sd_card_info(const sdmmc_card_t *card)
{
    sdmmc_card_print_info(stdout, card);
}

// Write sample data to the SD card
void write_sample_data(void)
{
    char file_path[100];
    uint8_t data_to_write[30] = {
        0x3B, 0x4C, 0x2A, 0x5E, 0x77, 0x88, 0xAA, 0xBB, 0xCC, 0xDD,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA,
        0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11, 0x22, 0x33, 0x44};

    for (int i = 0; i < 600; i++)
    {
        snprintf(file_path, sizeof(file_path), "%s/data%d.txt", MOUNT_POINT, history_counter);
        esp_err_t ret = write_data_to_sd_card(file_path, data_to_write, sizeof(data_to_write));
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to write data to file %s", file_path);
            continue;
        }

        ESP_LOGI(TAG, "Data written to file: %s", file_path);
        history_counter++;
    }
}

// Read and print historical data from the SD card
void read_historical_data(void)
{
    char file_path[100];
    uint8_t data_to_read[30];

    for (int i = 1; i < history_counter; i++)
    {
        snprintf(file_path, sizeof(file_path), "%s/data%d.txt", MOUNT_POINT, i);
        esp_err_t ret = read_data_from_sd_card(file_path, data_to_read, sizeof(data_to_read));
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to read data from file: %s", file_path);
            continue;
        }

        ESP_LOGI(TAG, "Data from file %s:", file_path);
        for (int j = 0; j < sizeof(data_to_read); j++)
        {
            printf("%02X ", data_to_read[j]);
        }
        printf("\n");
    }

    history_counter = 1; // Reset history counter after reading
}

// Main application entry point
void app_main(void)
{
    sdmmc_card_t *card;

    // Initialize SD card and check for errors
    esp_err_t ret = initialize_sd_card(&card);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "SD card initialization failed");
        return;
    }

    // Print SD card details
    print_sd_card_info(card);

    // Write sample data to SD card
    write_sample_data();

    // Read and print historical data from SD card
    read_historical_data();

    // Unmount the SD card
    ESP_LOGI(TAG, "Unmounting SD card");
    esp_vfs_fat_sdmmc_unmount();
    ESP_LOGI(TAG, "SD card unmounted successfully");
}
