#include "buzz_task.h"
#include "sntp_task.h"

#include "driver/ledc.h"

#include "pitches.h"

static const char *TAG = "buzz_task";

extern time_t sntp_now;
extern struct tm sntp_timeinfo;
extern struct tm alarm_timeinfo;

#define GPIO_OUTPUT_SPEED LEDC_HIGH_SPEED_MODE
const int BUZZER_PIN = 19;

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          BUZZER_PIN // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY               (2048) // Set duty to 50%. (2 ** 13) * 50% = 4096
#define LEDC_FREQUENCY          (400) // Frequency in Hertz

#define BUZZ_DELAY_S 1

// notes in the melody:
int melody[] = {
        NOTE_E5, NOTE_E5, NOTE_E5,
        NOTE_E5, NOTE_E5, NOTE_E5,
        NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5,
        NOTE_E5,
        NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5,
        NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5,
        NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5,
        NOTE_D5, NOTE_G5
};

// note durations: 4 = quarter note, 8 = eighth note, etc, also called tempo:
int noteDurations[] = {
        8, 8, 4,
        8, 8, 4,
        8, 8, 8, 8,
        2,
        8, 8, 8, 8,
        8, 8, 8, 16, 16,
        8, 8, 8, 8,
        4, 4
};

static void ledc_init(void) {
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
            .speed_mode       = LEDC_MODE,
            .duty_resolution  = LEDC_DUTY_RES,
            .timer_num        = LEDC_TIMER,
            .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 4 kHz
            .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
            .gpio_num       = BUZZER_PIN,
            .speed_mode     = LEDC_MODE,
            .channel        = LEDC_CHANNEL,
            .intr_type      = LEDC_INTR_DISABLE,
            .timer_sel      = LEDC_TIMER,
            .duty           = 0, // Set duty to 0%
            .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}


void buzz_task(void *parm) {
    alarm_timeinfo.tm_min = 99; // disable alarm at startup
    ledc_init();

    while (true) {

        time(&sntp_now);

        char strftime_buf[64];
        localtime_r(&sntp_now, &sntp_timeinfo);

        sprintf(strftime_buf, "CURNT: %02d:%02d:%02d", sntp_timeinfo.tm_hour % 12, sntp_timeinfo.tm_min,
                sntp_timeinfo.tm_sec);

        ESP_LOGI(strftime_buf, "");

        sprintf(strftime_buf, "ALARM: %02d:%02d:%02d", alarm_timeinfo.tm_hour % 12, alarm_timeinfo.tm_min,
                alarm_timeinfo.tm_sec);

        ESP_LOGI(strftime_buf, "");

        if (memcmp(&sntp_timeinfo, &alarm_timeinfo, sizeof(struct tm)) == 0) {
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY));
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
        }

        /*ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));

        vTaskDelay((1000 / portTICK_PERIOD_MS) * BUZZ_DELAY_S);

        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));

        vTaskDelay((1000 / portTICK_PERIOD_MS) * BUZZ_DELAY_S);*/
        vTaskDelay((1000 / portTICK_PERIOD_MS) * BUZZ_DELAY_S);
    }
}
