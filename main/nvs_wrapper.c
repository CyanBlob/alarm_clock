//
// Created by andre on 9/1/2024.
//

#include <esp_err.h>
#include <nvs_flash.h>
#include "nvs_wrapper.h"

static nvs_handle_t _nvs_handle;

void nvs_init()
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void nvs_set(char* key, char* value)
{
    printf("Updating %s in nvs ... \n", key);
    esp_err_t err = nvs_set_str(_nvs_handle, key, value);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
}

void nvs_get(char* key, char* value, size_t* len)
{
    printf("Getting %s from nvs\n", key);

    __attribute__((unused)) esp_err_t err = nvs_get_str(_nvs_handle, key, value, &len);
    printf("%s: %s\n", key, value);
}

bool nvs_begin()
{
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &_nvs_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return false;
    }
    return true;
}

void nvs_end()
{
    printf("Committing updates in NVS ... ");
    esp_err_t err = nvs_commit(_nvs_handle);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

    nvs_close(_nvs_handle);
}