#include "sntp_task.h"

static const char *TAG = "sntp_task";
time_t sntp_now;
struct tm sntp_timeinfo;

#define SYNC_DELAY_S 10 * 60

time_t currentTime() {
    return sntp_now;
}

struct tm currentTimeInfo() {
    return sntp_timeinfo;
}

void sntp_task(void *parm) {
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);

    while(true) {

        ESP_LOGI(TAG, "Syncing time...");

        if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(20000)) != ESP_OK) {
            printf("Failed to update system time within 20s timeout");
            continue;
        }


        time(&sntp_now);
        // Set timezone to Chicago Standard Time
        setenv("TZ", "UTC-7", 1);
        tzset();

        char strftime_buf[64];
        localtime_r(&sntp_now, &sntp_timeinfo);
        strftime(strftime_buf, sizeof(strftime_buf), "%c", &sntp_timeinfo);
        ESP_LOGI(TAG, "The current date/time in Chicago is: %s", strftime_buf);

        vTaskDelay((1000 / portTICK_PERIOD_MS) * SYNC_DELAY_S);
    }
}