#ifndef ALARM_CLOCK_SNTP_TASK_H
#define ALARM_CLOCK_SNTP_TASK_H

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <esp_netif_sntp.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "nvs_flash.h"
#include "esp_random.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_now.h"
#include "esp_crc.h"

time_t currentTime();
struct tm currentTimeInfo();
void sntp_task(void *parm);

#endif //ALARM_CLOCK_SNTP_TASK_H
