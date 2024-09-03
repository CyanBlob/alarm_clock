#include <stdio.h>
#include <inttypes.h>
#include <hal/wdt_hal.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "espnow_example.h"

#include "driver/gpio.h"

#include <Adafruit_Protomatter.h>

uint8_t rgbPins[]  = {25, 26, 27, 14, 15, 13};
uint8_t addrPins[] = {23, 22, 5, 2};
uint8_t clockPin   = 21; // Must be on same port as rgbPins
uint8_t latchPin   = 4;
uint8_t oePin      = 15;

Adafruit_Protomatter matrix(
        64,          // Width of matrix (or matrix chain) in pixels
        4,           // Bit depth, 1-6
        1, rgbPins,  // # of matrix chains, array of 6 RGB pins for each
        4, addrPins, // # of address pins (height is inferred), array of pins
        clockPin, latchPin, oePin, // Other matrix control pins
        false);      // No double-buffering here (see "doublebuffer" example)

extern "C" void app_main()
{
    printf("Hello world!\n");
    wdt_hal_context_t rtc_wdt_ctx = RWDT_HAL_CONTEXT_DEFAULT();
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_disable(&rtc_wdt_ctx);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);

    ProtomatterStatus status = matrix.begin();

    // Make four color bars (red, green, blue, white) with brightness ramp:
    for(int x=0; x<matrix.width(); x++) {
        uint8_t level = x * 256 / matrix.width(); // 0-255 brightness
        matrix.drawPixel(x, matrix.height() - 4, matrix.color565(level, 0, 0));
        matrix.drawPixel(x, matrix.height() - 3, matrix.color565(0, level, 0));
        matrix.drawPixel(x, matrix.height() - 2, matrix.color565(0, 0, level));
        matrix.drawPixel(x, matrix.height() - 1,
                         matrix.color565(level, level, level));
    }
    // You'll notice the ramp looks smoother as bit depth increases
    // (second argument to the matrix constructor call above setup()).

    // Simple shapes and text, showing GFX library calls:
    matrix.drawCircle(12, 10, 9, matrix.color565(255, 0, 0));
    matrix.drawRect(14, 6, 17, 17, matrix.color565(0, 255, 0));
    matrix.drawTriangle(32, 9, 41, 27, 23, 27, matrix.color565(0, 0, 255));

    matrix.write("TEST");
    matrix.print("ABC");

    //matrix.println("ADAFRUIT"); // Default text color is white
    if (matrix.height() > 32) {
        matrix.setCursor(0, 32);
        //matrix.println("64 pixel"); // Default text color is white
        //matrix.println("matrix"); // Default text color is white
    }

    // AFTER DRAWING, A show() CALL IS REQUIRED TO UPDATE THE MATRIX!

    matrix.show(); // Copy data to matrix buffers

    //gpio_set_direction(STATUS_LED_Pin, GPIO_MODE_DEF_OUTPUT);
    //gpio_set_level(STATUS_LED_Pin, 0);

    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    espnow_app_main();

    /*for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();*/
}